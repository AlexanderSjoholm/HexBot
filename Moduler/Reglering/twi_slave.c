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
	//st�ll in bitrate
	//TWBR = 0x0A;
	//st�ll in TWI-srreg s� att den skickar ack n�r den addreseras
	TWCR = (1<<TWEN)|(1<<TWIE)|(0<<TWINT)|
		   (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|
           (0<<TWWC);
}




void interpret_data()
{
	//F�rsta byten i s�ndningen �r alltid f�r att
	//avg�ra vilken typ av data som skall skickas
	switch(data[0])
	{
		//Ladda in sensordata till data-variablen
		//s� att n�sta g�ng slaven addreseras s� kan den
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
			//Detta mottas efter det att styrenheten fullf�ljt sin senaste instuktion
			//Nu skall en ny koll efter v�ggar g�ras.
			look_for_walls();
			//Sedan skall n�sta r�relse best�mmas
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




//Tar ej emot n�gra general calls. �r villig hela tiden
//(s�ger inte nej till en s�ndning/mottagning)

ISR(TWI_vect)
{
	switch(TWSR & 0xF8)
	{
		// SLAVE RECEIVER
		//---------------------------------------------------------
		//SLA+W mottagen, ACK skickad
		case TW_SR_SLA_ACK:
			bytes = 0;
	
		//om ej data vill s�ndas
	
			//Tag emot data och skicka ACK
			TWCR |= (1<<TWINT)|(1<<TWEA); 
			TWCR &= ~(0<<TWSTO);                
			break;
	
		//SLA+W mottagen, ACK skickad, arbitration lost i master
		case TW_SR_ARB_LOST_SLA_ACK:
	
		//om ej data vill s�ndas
	
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

		//Stop eller upprepad start n�r man �r addreserad
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
