#include <avr/interrupt.h>
#include <twi_slave.h>
#include <adc.h>
#include <timer.h>
#include <reglering.h>


int main()
{
	TIMER0init();
	ADCinit();
	sei();
	read_sensors();
	
	//for(int i= 0; i < 0xFFFF; i++);
	TWIslave_init();
	for(;;);
	return 0;
}
