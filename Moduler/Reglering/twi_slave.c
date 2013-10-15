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
#include <reglering.h>

//Initiering, antal bytes mottagna eller skickade
unsigned char bytes = 0;

//Initiering av TWI:n
void TWIslave_init()
{
	TWAR = (SLAVE_ADRESS<<1);
	//ställ in bitrate
	//TWBR = 0x0A;
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
				data[i] = measurements[i][0];
			}
		}
		case SEND_MOVEMENT_DATA:
		{
			//Detta mottas efter det att styrenheten fullföljt sin senaste instuktion
			//Nu skall en ny koll efter väggar göras.
			look_for_walls();
			//Sedan skall nästa rörelse bestämmas
			movement_direction();
			unsigned char i;
			for(i = 0; i < 6; i++)
			{
				data[i] = movement_data[i];
			}
		}
		case MOVEMENT_COMPLETE:
		{
			
		}
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
			TWCR &= ~(0<<TWSTO);                
			break;
	
		//SLA+W mottagen, ACK skickad, arbitration lost i master
		case TW_SR_ARB_LOST_SLA_ACK:
	
		//om ej data vill sändas
	
			//Tag emot data och skicka ACK
			TWCR |= (1<<TWINT)|(1<<TWEA);
			TWCR &= ~(0<<TWSTO);                
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
				TWCR &= ~(0<<TWSTO);
				bytes += 1;
			}
		break; 

		//Stop eller upprepad start när man är addreserad
		case TW_SR_STOP:
			
			//Behandla meddelandet
			interpret_data();
			//Tag emot framtida meddelanden
			TWCR |= (1<<TWINT)|(1<<TWEA);
			TWCR &= ~(0<<TWSTO);
			
			
		break; 

		//---------------------------------------------------------
		// SLAVE TRANSMITTER
		//---------------------------------------------------------
	
		//Egen SLA+R mottagen, ACK skickad
		case TW_ST_SLA_ACK:
			bytes = 0;
			//Skicka data och tag emot ack
			TWDR = data[bytes];

			TWCR |= (1<<TWINT)|(1<<TWEA); 
			TWCR &= ~(0<<TWSTO);
			bytes += 1;               
		break;
			//Data skickad, ACK mottagen
		case TW_ST_DATA_ACK:
			//Skicka data och tag emot ack
			if(bytes < MAX_DATA_BYTES )
			{
				TWDR = data[bytes];
				TWCR |= (1<<TWINT)|(1<<TWEA);
				TWCR &= ~(0<<TWSTO);
				bytes += 1;
			}
			/*else
			{
				TWDR = data[bytes];
				TWCR |= (1<<TWINT)|(0<<TWEA);
				TWCR &= ~(0<<TWSTO);
				bytes = 0;
			}
			*/
		break;
		case TW_ST_LAST_DATA:
			//Tag emot framtida meddelanden
			TWCR |= (1<<TWINT)|(1<<TWEA);
			TWCR &= ~(0<<TWSTO);
		break;
		default:
			TWCR &= ~(0<<TWSTO);
		break;
				
	}
}
