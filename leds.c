#include <s3c44b0x.h>
#include <leds.h>

#define LEFT_LED (1)
#define RIGHT_LED (2)
static uint8 state1;
static uint8 state2;

void leds_init( void )
{
    led_off(LEFT_LED);
    led_off(RIGHT_LED);
    state1 = 0;
    state2 = 0;
}

void led_on( uint8 led )
{
	if(led == LEFT_LED){
		PDATB &= ~(1<<9);
		state1 = 1;
	}
	else if(led == RIGHT_LED){
		PDATB &= ~(1<<10);
		state2 = 1;
	}
}

void led_off( uint8 led )
{
	if(led == LEFT_LED){
		PDATB |= (1<<9);
		state1 = 0;
	}
	else if(led == RIGHT_LED){
		PDATB |= (1<<10);
		state2 = 0;
	}
}

void led_toggle( uint8 led )
{
	if(led == LEFT_LED){
		PDATB ^= (1<<9);
		state1 ^= state1;
	}
	else if(led == RIGHT_LED){
		PDATB ^= (1<<10);
		state2 ^= state2;
	}
}

uint8 led_status( uint8 led )
{
	//uint8 state;

	if(led == LEFT_LED)
		/*if(PCONB & (1<<9))
			state = 0;
		else
			state = 1;*/
		return state1;
	else if(led == RIGHT_LED)
		/*if(PCONB & (1<<10))
			state = 0;
		else
			state = 1;*/
		return state2;

	//return state;
	return 0;
}

