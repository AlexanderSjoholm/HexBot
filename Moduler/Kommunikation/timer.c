#include <avr/interrupt.h>
#include <timer.h>
#include <twi.h>
#include <usart.h>

uint8_t send_data[6] = {0};
int timersteg = 0;

void TIMER0init()
{
	//DDRA = 0xFF; ; //set portd as output
	//ställ in timer-prescaler till clk/1024
	TCCR0 |= (1<<CS00)|(1<<CS02);

	//enable interruptflagga ifall timern får
	//overflow
	TIMSK |= (1<<TOIE0);
}

ISR(TIMER0_OVF_vect)
{
	switch(timersteg)
		{
			case 0:		// begär sensordata från sensorenheten 
			{
				data[0] = RECEIVE_SENSOR_DATA;
				if(i2c_writebyte(0x02))
				{
					timersteg = 1;
				}
				break;
			}
			case 1:		// ta emot och skicka sensordata till PC
			{
				if(i2c_readbyte(0x02))
				{
					
					send_data[0] = data[0];
					send_data[1] = data[1];
					send_data[2] = data[2];
					send_data[3] = data[3];
					send_data[4] = data[4];
					send_data[5] = data[5];
					USART_Transmit(send_data, sensor_datatyp, 6);
					timersteg = 2;
				}
				break;
			}
			//Kolla om styrenheten har gått klart
			case 2:		
				if(i2c_readbyte(0x01))
				{
					if(data[0] == 1)
					{
						timersteg = 3;
					}
					else
					{
						timersteg = 0;
					}
				}
				break;
			case 3:	// begär styrdata från sensorenheten 
			{		// ÄR PA0 switch mellan autonom och manuell???
				//if(PA0 == 1) // om PA0=1 ,dvs autonomt läge => begär data från sensor
				//{	
					data[0] = RECEIVE_MOVEMENT_DATA;
					if(i2c_writebyte(0x02))
					{	
						timersteg = 4;
					}
				//}
				//else timersteg = 0;
				break;
			}
			case 4:	// ta emot och skicka styrdata till STYRENHET
			{
				if(i2c_readbyte(0x02))
				{
					i2c_writebyte(0x01); 
					timersteg = 0;
				}
				break;
			}

	
		//	case 2:		// begär positionsdata från sensorenheten 
		/*	{
				data[0] = RECEIVE_POSITION_DATA;
				if(i2c_writebyte(0x02))
				{
					timersteg = 3;
				}
				break;
			}	*/

	//		case 3: 	// ta emot och skicka positionsdata till PC
	/*		{
				if(i2c_readbyte(0x02))
					{
					timersteg = 4;
					}
				break;
			}*/

	//		case 4:	// begär kartdata från sensorenheten 
	/*		{
				data[0] = RECEIVE_MAP_DATA;
				if(i2c_writebyte(0x02))
				{
					timersteg = 5;
				}
				break;
			} */
		
	//		case 5:	// ta emot och skicka kartdata till PC
	/*		{
				if(i2c_readbyte(0x02))
					{
					timersteg = 6;
					}
				break;
			}  */
			default:
			{
				timersteg = 0;
				break;
			}
		}
}
