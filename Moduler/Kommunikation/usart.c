#include <avr/io.h>
#include <avr/interrupt.h>
#include <usart.h>
#include <twi.h>

uint8_t nuvarande_bytes = 0;
uint8_t data_typ = 0;
uint8_t antal_bytes = 0;
uint8_t byte ; 
int steg = 0;
int sandning_typ = 0;
unsigned char styr_adress = 1;


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

uint8_t GetDataType(uint8_t byte)
{
return (byte>>4);
}
uint8_t GetNumberOfBytes(uint8_t byte)
{
return (byte & 0b00001111);
}

ISR(USART_TXC_vect)
{
	PORTD = (0<<PD5);
}


//receive interrupt 
ISR(USART_RXC_vect)
{
//	if(PA0==0)
//	{ // om PA0 = 0 , dvs manuell => gör som PC säger
		byte = UDR;
		switch (steg)
		{
		case 0: // Leta efter start
			{
				if(byte == 122)
				{
					steg = 1;
				}
				break;
			}
		case 1: // Ta ut datatyp och antal bytes.
			{
				data_typ = GetDataType(byte);
				antal_bytes = GetNumberOfBytes(byte);
				buffer_data[nuvarande_bytes++] = byte;
				steg = 2;
				break;
			}
		case 2: // Spara data
			{
				if(nuvarande_bytes == antal_bytes )
				{
					buffer_data[nuvarande_bytes] = (byte & 0b10111111);
					steg = 3;
				}
				else
				{
					buffer_data[nuvarande_bytes++] = (byte & 0b10111111);
				}
				break;
			}
		case 3: // STOP
			{ 
				if(byte == 126)
				{
				data[0] = buffer_data[0];
				data[1] = buffer_data[1];
				data[2] = buffer_data[2];
				data[3] = buffer_data[3];
				data[4] = buffer_data[4];
				data[5] = buffer_data[5];	 
					// Hantera mottagningen
				if(i2c_writebyte(styr_adress))
				{
					steg = 0;
				} // Allt som skickas ifrån datorn via blåtand ska alltid gå till styrenheten
				
				}
				steg = 0; // Nollställ mottagningsskvensen
				nuvarande_bytes = 0;
				break;
			}
		default:
			{
				steg = 0; // Nollställ mottagning
			}
		}
//	}
	
}


void USART_Transmit(uint8_t send_data[], uint8_t datatyp, uint8_t bytes)
{
	// Wait for empty transmit buffer 
	while ( !( UCSRA & (1<<UDRE)));
	PORTD=(1<<PD5);  // Robot ej redo att ta emot
	UDR = start_datatyp ;

	// Wait for empty transmit buffer 
	while ( !( UCSRA & (1<<UDRE)));
	PORTD=(1<<PD5);  // Robot ej redo att ta emot
	UDR = datatyp + bytes;

	uint8_t i;
	for(i = 0; i < bytes; i++)
	{
		while ( !( UCSRA & (1<<UDRE)));
		PORTD=(1<<PD5);  // Robot ej redo att ta emot
		UDR = send_data[i] ;
	}
	
	// Wait for empty transmit buffer 
	while ( !( UCSRA & (1<<UDRE)));
	PORTD=(1<<PD5);  // Robot ej redo att ta emot
	UDR = slut_datatyp ;

}
