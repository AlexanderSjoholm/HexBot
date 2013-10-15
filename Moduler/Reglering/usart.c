#include <avr/io.h>
#include <avr/interrupt.h>
#include <usart.h>



void USART_Init ()
{
	//Set baud rate
	UBRRL=MYUBRR;  //low byte
	UBRRH=((MYUBRR)>>8); //high byte
	//Set data frame format: synchronous mode,no parity, 1 stop bit, 8 bit
	UCSRC=(1<<URSEL)|(0<<UMSEL)|(0<<UPM1)|(0<<UPM0)|
		  (0<<USBS)|(0<<UCSZ2)|(1<<UCSZ1)|(1<<UCSZ0); 
	//Enable Transmitter and Receiver and Interrupt on receive complete
	UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);
	UCSRA=(1<<U2X); 
	PORTD=(0<<PD5);

}

//receive interrupt 
ISR(USART_RXC_vect)
	{
	}

ISR(USART_TXC_vect)  // transmit interrupt
	{
	}
 
void USART_Transmit( unsigned char data )  // data som ska skickas 
{
	// Wait for empty transmit buffer 
	while ( !( UCSRA & (1<<UDRE))); //PD4 CTS, PD5 RTS 
	PORTD=(1<<PD5);
	UDR = data;   //datan skickas på UDR a.k.a. USART-porten 
}

/*
int main (void)
{
	//enable global interrupts
	sei();
 
	while(1)
	{
		USART_Transmit(0x11);
	}
return 0;

}
*/
