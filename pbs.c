
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <pbs.h>
#include <timers.h>

extern void isr_PB_dummy( void );

void pbs_init( void )
{
    timers_init();
}

uint8 pb_scan( void )
{
    if( (~PDATG & PB_LEFT) )
        return PB_LEFT;
    else if( (~PDATG & PB_RIGHT) )
        return PB_RIGHT;
    else
        return PB_FAILURE;
}

uint8 pb_pressed( void )
{

	if(pb_scan() == PB_FAILURE)
		return PB_UP;

	return PB_DOWN;
}

uint8 pb_getchar( void )
{
	uint8 scancode;

	while( !pb_pressed() );
	sw_delay_ms( PB_KEYDOWN_DELAY );

	scancode = pb_scan();

	while( pb_pressed() );
	sw_delay_ms( PB_KEYUP_DELAY );

	return scancode;
}

uint8 pb_getchartime( uint16 *ms )
{
    uint8 scancode;
    
    while( !pb_pressed() );
    timer3_start();
    sw_delay_ms( PB_KEYDOWN_DELAY );
    
    scancode = pb_scan();
    
    while( pb_pressed() );
    *ms = timer3_stop() / 10;
    sw_delay_ms( PB_KEYUP_DELAY );

    return scancode;
}

uint8 pb_timeout_getchar( uint16 ms )
{
	uint8 scancode;

	timer3_start_timeout(ms);

	while( !pb_pressed() && !timer3_timeout());

	if(!timer3_timeout()){
		sw_delay_ms( PB_KEYDOWN_DELAY );

		scancode = pb_scan();

		timer3_start_timeout(ms);
		while( pb_pressed() && !timer3_timeout());
		if(!timer3_timeout())
			sw_delay_ms( PB_KEYUP_DELAY );
	}
	else{
		scancode = PB_TIMEOUT;
	}


	return scancode;
}

void pbs_open( void (*isr)(void) )
{
    pISR_PB   = (uint32) isr;
    EXTINTPND = EXTINTPND | 3 << 2;
    I_ISPC    = I_ISPC | (1 << 21);
    INTMSK   &= ~((1 << 26) | (1<<21));
}

void pbs_close( void )
{
    INTMSK  |= (1<<21);
    pISR_PB  =  (uint32) isr_PB_dummy;
}
