#include <avr/interrupt.h>

#include <main.h>
#include <twi.h>
#include <usart.h>
#include <timer.h>


int main()
{
	TIMER0init();
	twi_init();	
	USART_Init();
	sei();
	for(;;);
	return 0;
}
