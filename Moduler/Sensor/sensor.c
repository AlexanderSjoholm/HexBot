#include<avr/io.h>
#include<stdlib.h>
#include <avr/interrupt.h>

#define no_mode 0x00
#define reg_mode 0x01
#define map_mode 0x02
#define read_mode 0x03

#define max_number_reads 4
#define side_wall_dist_1 0x50
#define wall_dist_1 0x59

unsigned char adc_ch = 0;
volatile unsigned char mode = 0;
unsigned char measurements[6][4];
unsigned char reads = 0;
unsigned char mesurement = 0;


void init_adcreg()
{
	DDRD = 0xFF; ; //set portd as output
	ADCSRA |= (1<<ADEN)|(1<<ADIE)|(1<<ADPS1)|(1<<ADPS0); // set adc-prescaler, set adc enable and adc interrupt enable
	ADMUX |= (1<<REFS0) | (1 << ADLAR); // set adv vcc ref
	ADMUX |= adc_ch;
}

//läser av alla ir-sensorer och sparar ner dessa
void read_sensors()
{
	mode = read_mode;
	adc_ch = 0;
	ADMUX &= 0xE0;
	ADCSRA |= (1 << ADSC);

}

unsigned char get_measurements(unsigned char sensor, unsigned char row)
{
	return measurements[sensor][row];
}

void set_measurements(unsigned char sensor, unsigned char row, unsigned char data)
{
	measurements[sensor][row] = data;
}

unsigned char decide_side_wall()
{
	read_sensors();

	//Vänta tills alla avläsningar är klara
	while(mode != no_mode);
	
	unsigned char i = 0;
	unsigned char value = abs((get_measurements(0, 0) / 2) + (get_measurements(5, 0) / 2) - side_wall_dist_1);
	unsigned char direction = 1;
	
	//Då igenom alla sensorer
	for(i = 0; i < 5; i++)
	{
		unsigned char temp;
		temp = abs((get_measurements(i + 1, 1) / 2) + (get_measurements(i, 1) / 2) - side_wall_dist_1);
		
		if(temp < value)
		{
			value = temp;
			direction = i + 2;
		}
		
		
	}

	//Som direction % 6
	if(direction == 6)
	{
		return 0;
	}
	return direction;	
}

int main()
{	
	init_adcreg();
	sei(); //enable interrupts
		
	for(;;)
	{
		read_sensors();
		while(mode != no_mode)
		{}

		
	}
	return 0;
}

ISR(ADC_vect)
{
	
	switch(mode)
	{	
		case read_mode:
		{
			if(reads < max_number_reads)
			{
				unsigned char adc = ADCH;

				set_measurements(adc_ch, reads, adc);


				if(adc_ch < 6)
				{
					adc_ch += 1;
					ADMUX += 1;

				}
				else
				{
					reads += 1;
					adc_ch = 0;
					ADMUX &= 0xE0;
				}

				ADCSRA |= (1 << ADSC);
			}
			else
			{
				reads = 0;
				mode = no_mode;
			}
		break;
		}
	}
} 
