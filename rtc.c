
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <rtc.h>

extern void isr_TICK_dummy( void );

void rtc_init( void )
{
    TICNT   |= 1<<7;
    RTCALM  = 0x0;
    RTCRST  = 0x0;
        
    RTCCON  |= 1<<0;
    
    BCDYEAR = 0x13;
    BCDMON  = 0x01;
    BCDDAY  = 0x01;
    BCDDATE = 0x01;
    BCDHOUR = 0x00;
    BCDMIN  = 0x00;
    BCDSEC  = 0x00;

    ALMYEAR = 0x0;
    ALMMON  = 0x0;
    ALMDAY  = 0x0;
    ALMHOUR = 0x0;
    ALMMIN  = 0x0;
    ALMSEC  = 0x0;

    RTCCON &= ~(1<<0);
}

uint8 bcd_to_binary(uint8 number){
	uint8 result = 0;

	result+=(number>>4)*10;
	result+= number&0x0f;

	return result;
}

uint8 binary_to_bcd(uint8 number){
	uint8 result = 0;

	result+= number/10;
	result = result<<4;
	result+= number%10;

	return result;
}

void rtc_puttime( rtc_time_t *rtc_time )
{
    RTCCON |= 1<<0;
    
    BCDYEAR = binary_to_bcd(rtc_time->year);
    BCDMON  = binary_to_bcd(rtc_time->mon);
    BCDDAY  = binary_to_bcd(rtc_time->mday);
    BCDDATE = binary_to_bcd(rtc_time->wday);
    BCDHOUR = binary_to_bcd(rtc_time->hour);
    BCDMIN  = binary_to_bcd(rtc_time->min);
    BCDSEC  = binary_to_bcd(rtc_time->sec);
        
    RTCCON &= ~(1<<0);
}

void rtc_gettime( rtc_time_t *rtc_time )
{
    RTCCON |= 1<<0;
    
    rtc_time->year = bcd_to_binary(BCDYEAR);
    rtc_time->mon  = bcd_to_binary(BCDMON);
    rtc_time->mday = bcd_to_binary(BCDDAY);
    rtc_time->wday = bcd_to_binary(BCDDATE);
    rtc_time->hour = bcd_to_binary(BCDHOUR);
    rtc_time->min  = bcd_to_binary(BCDMIN);
    rtc_time->sec  = bcd_to_binary(BCDSEC);
    if( ! rtc_time->sec ){
    	rtc_time->year = bcd_to_binary(BCDYEAR);
		rtc_time->mon  = bcd_to_binary(BCDMON);
		rtc_time->mday = bcd_to_binary(BCDDAY);
		rtc_time->wday = bcd_to_binary(BCDDATE);
		rtc_time->hour = bcd_to_binary(BCDHOUR);
		rtc_time->min  = bcd_to_binary(BCDMIN);
		rtc_time->sec  = bcd_to_binary(BCDSEC);
    };

    RTCCON &= ~(1<<0);
}


void rtc_open( void (*isr)(void), uint8 tick_count )
{
    pISR_TICK	= 	(uint32) isr;
    I_ISPC		&= 	~(1<<20);
    INTMSK		&= 	~(1<<26 | (1<<20));
    TICNT		= 0xff;
}

void rtc_close( void )
{
    TICNT     	= 0x0;
    INTMSK   	|= 1<<20;
    pISR_TICK 	= (uint32) isr_TICK_dummy;
}
