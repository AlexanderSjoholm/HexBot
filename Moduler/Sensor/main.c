#include <avr/interrupt.h>
#include <twi_slave.h>
#include <adc.h>
#include <reglering.h>

int main()
{

	ADCinit();
	sei();
	read_sensors();
	TWIslave_init();

	//-------------------------------
	//		Lite Test
	//-------------------------------

	DDRB = 0xFF;
	 
	direction = 0;
	for(;;)
	{	
		look_for_walls();
		PORTB = get_local_wall(1,1);
	}
	

	return 0;
}
