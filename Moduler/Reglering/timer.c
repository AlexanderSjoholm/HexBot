#include <avr/interrupt.h>
#include <timer.h>
#include <adc.h>

void TIMER0init()
{
	//st�ll in timer-prescaler till clk/1024
	TCCR0 |= (1<<CS00)|(1<<CS02);

	//enable interruptflagga ifall timern f�r
	//overflow
	TIMSK |= (1<<TOIE0);
}

ISR(TIMER0_OVF_vect)
{
	read_sensors();

}

