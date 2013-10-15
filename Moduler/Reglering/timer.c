#include <avr/interrupt.h>
#include <timer.h>
#include <adc.h>

void TIMER0init()
{
	//ställ in timer-prescaler till clk/1024
	TCCR0 |= (1<<CS00)|(1<<CS02);

	//enable interruptflagga ifall timern får
	//overflow
	TIMSK |= (1<<TOIE0);
}

ISR(TIMER0_OVF_vect)
{
	read_sensors();

}

