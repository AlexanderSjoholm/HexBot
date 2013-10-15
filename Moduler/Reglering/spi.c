#include<avr/io.h>
#include<avr/interrupt.h>

/* to get a value from the mlx90609 you need to first
initialize the spi and activate the adc. -> delay 115µs ->
*/


/* Function to initialize the SP */
void init_spi()
{
//Set IO-directions
DDRB |= (1<<DDB4) | // SS output
		(1<<DDB5) | // MOSI output
		(1<<DDB7);  // SCK output

//SPI settings
SPCR |= (1<<SPE) |  //SPI enable
		(1<<MSTR) | //Set device as master
		(1<<CPHA) |
		(1<<CPOL);  //SCLK high when idle

PORTB |= (1<<PORTB4); //Set SS high

}


/* Function to send a message as a master */
void send_spi(unsigned char message)
{
	SPDR =  message;
	while(!(SPSR & (1<<SPIF)))
	{}
}

unsigned char read_spi(unsigned char message)
{
	SPDR =  message;
	while(!(SPSR & (1<<SPIF)))
	{}
	return SPDR;
}
/* Activates the adc on the mlx90609 if in sleep-mode */
void activate_ar_sensor()
{
	PORTB &= ~(1<<PORTB4); //Set SS low
	send_spi(0x94); //ADCC for angular rate
	PORTB |= (1<<PORTB4); //Set SS high
}

/* Reads the angular-rate from the mlx90609 */
unsigned short read_ar()
{
	unsigned char data_h, data_l;

	PORTB &= ~(1<<PORTB4); //Set SS low
	send_spi(0x80); // ADC reading
	data_h = read_spi(0x00); //get sensor high byte
	data_l = read_spi(0x00); //get sensor low byte 
	PORTB |= (1<<PORTB4); //Set SS high

	//return (((data_h & 0x0F) << 8) + data_l);

	return ((data_h & 0x0F) << 8) + data_l ;
}

int main()
{

sei();

init_spi();

activate_ar_sensor();


//for(;;)
//{
unsigned short int temp = read_ar();
PORTA = temp & 0x00FF;
PORTD = temp >> 8;
//}

return 0;
}
