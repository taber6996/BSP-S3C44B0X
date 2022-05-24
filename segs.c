
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <segs.h>

static const uint8 hex2segs[16] = {0x12, 0x9F, 0x31, 0x15, 0x9C, 0x54, 0x50, 0x0F, 0x10, 0x1C, 0x18, 0x00, 0x62, 0x02, 0x60, 0x68};

static uint8 state;

void segs_init( void )
{
    segs_off();
}

void segs_off( void )
{
    state = SEGS_OFF;
    SEGS = state;
}

void segs_putchar( uint8 n )
{
    state = n & 0x0f;
    SEGS = hex2segs[state];
};

uint8 segs_status( void )
{
    return state;
};

