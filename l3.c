
#include <s3c44b0x.h>
#include <l3.h>
#include <leds.h>

#define SHORT_DELAY    { int8 j; for( j=0; j<4; j++ ); }

void L3_init( void )
{
    PDATB = 3 << 4;//PDATB[4] Y PDATB[5] a 1
}

void L3_putByte( uint8 byte, uint8 mode )
{
    uint8 i;
    uint8 rled, lled;
    
    rled = !led_status( RIGHT_LED );
    lled = !led_status( LEFT_LED );    
   
    PDATB = (PDATB & ~(3 << 4)) | 1 << 5 | mode << 4;
    SHORT_DELAY;

    for( i=0; i<8; i++ )
    {
        PDATB = (PDATB & ~(3 << 4)) | mode << 4;
        PDATA = (PDATA & ~(1 << 9)) | byte%2 << 9;
        SHORT_DELAY;    
        PDATB = (PDATB & ~(3 << 4)) | 1 << 5 | mode << 4;
        SHORT_DELAY;
        byte/=2;
    }
    PDATB = (rled << 10) | (lled << 9) | (1 << 5) | (1 << 4);   
}

