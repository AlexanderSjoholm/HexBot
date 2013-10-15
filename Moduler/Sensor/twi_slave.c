//-------------------------------------------------------------------------------
//
//								TWI
//
//-------------------------------------------------------------------------------

//#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/twi.h>
#include <twi_slave.h>
#include <adc.h>

//Initiering, antal bytes mottagna eller skickade
volatile unsigned char bytes = 0;

unsigned char get_send_data(unsigned char index)
{
	return from_sensor_data[index];
}

void set_send_data(unsigned char index, unsigned char data)
{
	from_sensor_data[index] = data;
}



//Initiering av TWI:n
void TWIslave_init()
{
//	TWBR = 0x20;
	TWAR = (SLAVE_ADRESS<<1);
	//ställ in TWI-srreg så att den skickar ack när den addreseras
	TWCR = (1<<TWEN)|(1<<TWIE)|(0<<TWINT)|
		   (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|
           (0<<TWWC);
}




void interpret_data()
{
	//Första byten i sändningen är alltid för att
	//avgöra vilken typ av data som skall skickas
	switch(data[0])
	{
		//Ladda in sensordata till data-variablen
		//så att nästa gång slaven addreseras så kan den
		//skicka datan
		case SEND_SENSOR_DATA:
		{
			unsigned char i;
			for(i = 0; i < 6; i++)
			{
				set_send_data(i, measurements[i]);
			}
		}

		case SEND_MOVEMENT_DATA:
		{
			set_send_data(0,movement_data[0]);
		}
		break;

		case SEND_POSITION_DATA:
		{
			set_send_data(0,movement_data[0]);
		}
		break;
		case SEND_MAP_DATA:
		{
			set_send_data(1,map_data[0]);
		}
		break;

	}
}




//Tar ej emot några general calls. Är villig hela tiden
//(säger inte nej till en sändning/mottagning)

ISR(TWI_vect)
{
	switch(TWSR & 0xF8)
	{
		// SLAVE RECEIVER
		//---------------------------------------------------------
		//SLA+W mottagen, ACK skickad
		case TW_SR_SLA_ACK:
			bytes = 0;
		//om ej data vill sändas
	
			//Tag emot data och skicka ACK
			TWCR |= (1<<TWINT)|(1<<TWEA); 
			TWCR &= ~(1<<TWSTO);                
			break;
	
		//SLA+W mottagen, ACK skickad, arbitration lost i master
		case TW_SR_ARB_LOST_SLA_ACK:
	
		//om ej data vill sändas
	
			//Tag emot data och skicka ACK
			TWCR |= (1<<TWINT)|(1<<TWEA);
			TWCR &= ~(1<<TWSTO);                
			break;	
			//Bry dig ej om general calls
	
		//Data mottagen, ACK skickad
		case TW_SR_DATA_ACK:
				//Spara ner data
			data[bytes] = TWDR;
			
			//Om den kan ta emot mer data, skicka ack
			if(bytes < MAX_DATA_BYTES)
			{
				//Tag emot data och skicka ACK
				TWCR |= (1<<TWINT)|(1<<TWEA);
				TWCR &= ~(1<<TWSTO);
				bytes += 1;
			}
		break; 

		case TW_SR_DATA_NACK:
			//Spara ner data
			data[bytes] = TWDR;
			
			//Om den kan ta emot mer data, skicka ack
			if(bytes < MAX_DATA_BYTES)
			{
				//Tag emot data och skicka ACK
				TWCR |= (1<<TWINT)|(1<<TWEA);
				TWCR &= ~(1<<TWSTO);
				bytes += 1;
			}
	               
		break;

		//Stop eller upprepad start när man är addreserad
		case TW_SR_STOP:

			//Tag emot framtida meddelanden
			TWCR |= (1<<TWINT)|(1<<TWEA);
			TWCR &= ~(1<<TWSTO);
				
			//Behandla meddelandet
			interpret_data();

		break; 

		//---------------------------------------------------------
		// SLAVE TRANSMITTER
		//---------------------------------------------------------
	
		//Egen SLA+R mottagen, ACK skickad
		case TW_ST_SLA_ACK:
			bytes = 0;
			//Skicka data och tag emot ack
			TWDR = get_send_data(bytes);
			TWCR &= ~(1<<TWSTO);
			TWCR |= (1<<TWINT)|(1<<TWEA); 
	
			bytes += 1;               
		break;

		//Data skickad, ACK mottagen
		case TW_ST_DATA_ACK:
			//Skicka data och tag emot ack
			if(bytes < MAX_DATA_BYTES - 1)
			{
				TWDR = get_send_data(bytes);
				TWCR &= ~(1<<TWSTO);
				TWCR |= (1<<TWINT)|(1<<TWEA);
				
				bytes += 1;
			}
			else
			{
 				TWDR = get_send_data(bytes);//from_sensor_data[bytes];
				TWCR &= ~((1<<TWSTO) | (1<<TWEA));
				TWCR |= (1<<TWINT);
				bytes = 0;
				
			}
		break;

		case TW_ST_LAST_DATA:
			//Tag emot framtida meddelanden
			TWCR |= (1<<TWINT)|(1<<TWEA);
			TWCR &= ~(1<<TWSTO);
			read_sensors();
	
		break;

		default:
 			TWCR &= ~((1<<TWSTO)|(1<<TWEA));
			TWCR |= (1<<TWINT);
			
		break;
	}

}
