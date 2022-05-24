
#include <s3c44b0x.h>
#include <lcd.h>

extern uint8 font[];
static uint8 lcd_buffer[LCD_BUFFER_SIZE];

static uint8 state;

void lcd_init( void )
{      
    DITHMODE = 0x12210;
    DP1_2    = 0xA5A5;
    DP4_7    = 0xBA5DA65;
    DP3_5    = 0xA5A5F;
    DP2_3    = 0xD6B;
    DP5_7    = 0xEB7B5ED;
    DP3_4    = 0x7DBE;
    DP4_5    = 0x7EBDF;
    DP6_7    = 0x7FDFBFE;
    
    REDLUT   = 0X0;
    GREENLUT = 0X0;
    BLUELUT  = 0X0;

    LCDCON1  = 0x1C020;
    LCDCON2  = 0x13CEF;
    LCDCON3  = 0X0;

    LCDSADDR1 = (2 << 27) | ((uint32)lcd_buffer >> 1);
    LCDSADDR2 = (1 << 29) | (((uint32)lcd_buffer + LCD_BUFFER_SIZE) & 0x3FFFFF) >> 1;
    LCDSADDR3 = 0X50;
    
    lcd_off();
}

void lcd_on( void )
{
	LCDCON1 |= (1<<0);
	state = 1;
}

void lcd_off( void )
{
	LCDCON1 &= ~(1<<0);
	state = 0;
}

uint8 lcd_status( void )
{
    return state;
}

void lcd_clear( void )
{
	uint16 i;
	for(i = 0; i < LCD_BUFFER_SIZE;i++){
		lcd_buffer[i] = 0x0;
	}

}

void lcd_putpixel( uint16 x, uint16 y, uint8 c)
{
    uint8 byte, bit;
    uint16 i;

    i = x/2 + y*(LCD_WIDTH/2);
    bit = (1-x%2)*4;
    
    byte = lcd_buffer[i];
    byte &= ~(0xF << bit);
    byte |= c << bit;
    lcd_buffer[i] = byte;
}

uint8 lcd_getpixel( uint16 x, uint16 y )
{
	uint8 byte, bit;
	uint16 i;

	i = x/2 + y*(LCD_WIDTH/2);
	bit = (1-x%2)*4;// 0 ó 4??

	byte = lcd_buffer[i];
	if(bit == 4)
		byte >>= bit;
	else
		byte &= ~(0xF << 4);

	return byte;
}

void lcd_draw_hline( uint16 xleft, uint16 xright, uint16 y, uint8 color, uint16 width )
{
	uint8 width_aux = width;
    while(xleft <= xright){
    	width_aux--;
    	if(lcd_getpixel(xleft, y+width_aux) == 0)
    		lcd_putpixel(xleft, y+width_aux, color);

    	if(width_aux == 0){
    		xleft++;
    		width_aux = width;
    	}
    }
}

void lcd_draw_vline( uint16 yup, uint16 ydown, uint16 x, uint8 color, uint16 width )
{
	uint8 width_aux = width;
	while(yup <= ydown){
		width_aux--;
		if(lcd_getpixel(x+width_aux, yup) == 0)
			lcd_putpixel(x+width_aux, yup, color);

		if(width_aux == 0){
			yup++;
			width_aux = width;
		}
	}
}

void lcd_draw_box( uint16 xleft, uint16 yup, uint16 xright, uint16 ydown, uint8 color, uint16 width )
{
	//lcd_clear();
    lcd_draw_hline(xleft, xright, yup, color, width);
    lcd_draw_hline(xleft, xright, ydown-width+1, color, width);
    lcd_draw_vline(yup, ydown, xright-width+1, color, width);
    lcd_draw_vline(yup, ydown, xleft, color, width);

}

void lcd_putchar( uint16 x, uint16 y, uint8 color, char ch )
{
    uint8 line, row;
    uint8 *bitmap;

    bitmap = font + ch*16;
    for( line=0; line<16; line++ )
        for( row=0; row<8; row++ )                    
            if( bitmap[line] & (0x80 >> row) )
                lcd_putpixel( x+row, y+line, color );
            else
                lcd_putpixel( x+row, y+line, WHITE );
}

void lcd_puts( uint16 x, uint16 y, uint8 color, char *s )
{
	//lcd_clear();
	uint8 i = 0;
    while(s[i] != '\0'){
    		lcd_putchar(x, y, color, s[i]);
    		x+=8;
    		i++;
    }
}

void lcd_putint( uint16 x, uint16 y, uint8 color, int32 i )
{
	char numero[10 + 1 + 1];
	char *p = numero+12;
	char es_neg = 0;
	*p = '\0';

	if(i < 0){
		es_neg = '1';
		i*=-1;
	}

	do{
		*--p = '0' + i%10;
		i/=10;
	}while(i);

	if(es_neg)
		*--p = '-';

	lcd_puts(x, y, color, p);
}

void lcd_puthex( uint16 x, uint16 y, uint8 color, uint32 i )
{
	char buf[8 + 1];
	char *p = buf + 8;
	uint8 c;

	*p = '\0';

	do {
		c = i & 0xf;
		if( c < 10 )
			*--p = '0' + c;
		else
			*--p = 'A' + c - 10;
		i = i >> 4;
	} while( i );

	lcd_puts(x, y, color, p);
}

void lcd_putchar_x2( uint16 x, uint16 y, uint8 color, char ch )//multiplica el tamaño
{
	uint8 line, row, line_aux = 0, row_aux = 0;
	uint8 *bitmap;

	bitmap = font + ch*16;
	for( line=0; line<16; line++ ){
		for( row=0; row<8; row++ ){
			if( bitmap[line] & (0x80 >> row) ){
				lcd_putpixel( x+row_aux, y+line_aux, color );
				lcd_putpixel( x+row_aux, y+line_aux+1, color );
				row_aux++;
				lcd_putpixel( x+row_aux, y+line_aux, color );
				lcd_putpixel( x+row_aux, y+line_aux+1, color );
				row_aux++;
				//line_aux+=2;
				}
			else{
				lcd_putpixel( x+row_aux, y+line_aux, WHITE );
				lcd_putpixel( x+row_aux, y+line_aux+1, WHITE );
				row_aux++;
				lcd_putpixel( x+row_aux, y+line_aux, WHITE );
				lcd_putpixel( x+row_aux, y+line_aux+1, WHITE );
				row_aux++;
				//line_aux+=2;
			}
		}
		row_aux=0;
		line_aux+=2;
	}
}

void lcd_puts_x2( uint16 x, uint16 y, uint8 color, char *s )
{
	uint8 i = 0;
	//lcd_clear();
	while(s[i] != '\0'){
			lcd_putchar_x2(x, y, color, s[i]);
			x+=16;
			i++;
	}
}

void lcd_putint_x2( uint16 x, uint16 y, uint8 color, int32 i )
{
	char numero[10 + 1 + 1];
	char *p = numero+12;
	char es_neg = 0;
	*p = '\0';

	if(i < 0){
		es_neg = '1';
		i*=-1;
	}

	do{
		*--p = '0' + i%10;
		i/=10;
	}while(i);

	if(es_neg)
		*--p = '-';

	lcd_puts_x2(x, y, color, p);

}

void lcd_puthex_x2( uint16 x, uint16 y, uint8 color, uint32 i )
{
	char buf[8 + 1];
	char *p = buf + 8;
	uint8 c;

	*p = '\0';

	do {
		c = i & 0xf;
		if( c < 10 )
			*--p = '0' + c;
		else
			*--p = 'A' + c - 10;
		i = i >> 4;
	} while( i );

	lcd_puts_x2(x, y, color, p);
}

void lcd_putWallpaper( uint8 *bmp )
{
    uint32 headerSize;

    uint16 x, ySrc, yDst;
    uint16 offsetSrc, offsetDst;

    headerSize = bmp[10] + (bmp[11] << 8) + (bmp[12] << 16) + (bmp[13] << 24);

    bmp = bmp + headerSize;
    
    for( ySrc=0, yDst=LCD_HEIGHT-1; ySrc<LCD_HEIGHT; ySrc++, yDst-- )                                                                       
    {
        offsetDst = yDst*LCD_WIDTH/2;
        offsetSrc = ySrc*LCD_WIDTH/2;
        for( x=0; x<LCD_WIDTH/2; x++ )
            lcd_buffer[offsetDst+x] = ~bmp[offsetSrc+x];
    }
}
