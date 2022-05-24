//Alejandro Tabernero Pérez
#include <s3c44b0x.h>
#include <uart.h>

void uart0_init( void )
{
    UFCON0 = 0x1;
    UMCON0 = 0x0;
    ULCON0 = 0x3;
    UBRDIV0 = 0x22;
    UCON0 = 0x5;
}

void uart0_putchar( char ch )
{
    while( UFSTAT0 & (1<<9) );
	//while( (UFSTAT0 & (0xf0) >> 4) == 15);
    UTXH0 = ch;
}        

char uart0_getchar( void )
{
    while( (UFSTAT0 & (0xF)) == 0 );
    return URXH0;
}

void uart0_puts( char *s )
{
	uint8 i = 0;
    while(s[i] != '\0'){
    	uart0_putchar(s[i]);
    	i++;
    }
}

void uart0_putint( int32 i )
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
    uart0_puts(p);
}

void uart0_puthex( uint32 i )
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

    uart0_puts( p );
}

void uart0_gets( char *s )
{
	uint8 i = 255;

	do
	{
		i++;
		s[i] = uart0_getchar();
	}while(s[i] != '\n');
	s[i] = '\0';
}

int32 uart0_getint( void )
{
    int32 resul = 0;
    char s[11+1];
    char *p = s;

    uart0_gets(s);

    if(s[0] == '-')
    	p++;
    do{
    	resul+= (*p - '0');
    	if(*++p != '\0')
    		resul*=10;
    }while(*p != '\0');

    if(s[0] == '-')
        	resul *= -1;

    return resul;
}

uint32 uart0_gethex( void )
{
	int32 resul = 0;
	char s[8+1];
	char *p = s;

	uart0_gets(s);

	do{
		if(*p> 47 && *p < 58){
			resul+= (*p - '0');
		}
		else if((*p> 64 && *p < 71))
			resul+= (*p - 'A' + 10);
		else if((*p> 96 && *p < 103))
			resul+= (*p - 'a' + 10);
		if(*++p != '\0')
			resul*=16;
	}while(*p != '\0');

	return resul;
}

