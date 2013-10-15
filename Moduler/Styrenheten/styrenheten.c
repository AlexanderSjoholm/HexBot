#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/twi.h>
#include <avr/pgmspace.h>

#include <twi_slav.h>
#include <styrenheten.h>
#include <styrmatris.h>
#include <pwm.h>

/*	INNEH�LLSF�RTECKNING

	GLOBALA VARIABLER
	STYRKOD
	TWI_SLAVE-KOD
	main()
*/

//------------------------------------
//		GLOBALA VARIABLER
//------------------------------------

unsigned char movement_complete = 1;

//Uppdrag
// 0 = St� still
// 1 = G�
// 2 = Rotera medurs
// 3 = Rotera moturs
// 4 = Rotera medurs LITE
// 5 = Rotera moturs LITE
int uppdrag = 1;
//V�rdet p� position anger initialposition
int position = 3;

//CPU frekvens
//cpu_freq = 1000000;

//PWM r�knar topv�rde. /2 f�r att vi anv�nder 
//Phase correct pwm som r�knar upp till top sedan ner.
//Allts� kr�vs halva top v�rdet
//top = cpu_freq*(20ms)/9/2
const int top = 10000/9; //1111;

//Styrsignal till muxen
int mux_sig = 0;

//varv_tak best�mmer hur l�nge en position ska h�llas
//Hastighet - M�ste vara en multipel av 9
int varv_tak = 9*3; //9*5
int varv = 1;

//Anger vridning i f�rh�llande till grundposition, 0 -> 5
int riktning = 1;
// Variabler n�dv�ndiga f�r att kunna tolka kommandon ifr�n komm-enheten
int temp_riktning = 3;

//Grunkompensation f�r varje riktnings grundfel
//int kompensation[6] = {20,50,18,0,40,-15};
int kompensation[6] = {0,0,0,0,0,0};

// Reglering varieras mha I2C
int reglering = 0;
int steps = 0;

//Dessa initieras i styr_setup
int benA;
int benB;
int servo = 0;
int temp = 0;



//Funktion f�r att s�tta riktning s� att rikt_arg = 1 
//ger det som i verkligeheten k�nns som riktning 1
/*void set_riktning(char rikt_arg)
{
	riktning = 4 - rikt_arg;
	if(riktning < 0)
	{
		riktning += 6;
	}
	temp_riktning = riktning;
	uppdatera_ben_mux();
}*/

void uppdatera_ben_mux()
{
	benA = riktning;
	benB = benA + 1;
	ben_modulo();
	mux_sig = 0;
	servo = 0;
}

void ben_modulo()
{
	if(benA > 5)
		{
			benA = benA - 6;
		}
	if(benB > 5)
		{
			benB = benB - 6;
		}
}


//------------------------------------
//		STYRKOD
//------------------------------------

//Processor inst�llningar f�r PWM
void styr_setup(void)
{

	//Register inst�llningar
	TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0);
	TCCR1B = (1 << WGM13)| (1 << CS11);
	//Vi beh�ver inte Compare interrupt n�r Phase Correct anv�nds
	//Ty, pulserna centreras kring top allts� finns spelrum i slutet
	//f�r att hinna muxa
	TIMSK = (1 << TOIE1);
	ICR1 = top;

	OCR1A = pgm_read_word(&styrmatris[0][0][0]);
	OCR1B = pgm_read_word(&styrmatris[0][1][0]);

	//S�tter port D, pin 5,4 som ut f�r PWM
	//S�tter port A, pin 3,2,1,0 som ut. Till MUX, 
	DDRD = 0b00111111; 

	PORTB = mux_sig;

	benA = riktning;
	benB = benA + 1;
}


//AVBROTTSRUTIN 
//som kickar ig�ng n�r PWM-r�knaren g�r i taket.
ISR(TIMER1_OVF_vect)
{

	//R�knar upp muxsignalen och b�rjar om n�r den kommer till 9
	mux_sig++;
	if(mux_sig == 9)
	{
		mux_sig = 0;
	}
	PORTD = mux_sig;

	//stegar igenom styrmatrisen s� att r�tt signal skickas till r�tt servo
	servo++;
	if(servo == 3)
	{
		servo = 0;
		benA = benA + 2;
		benB = benA + 1;
		ben_modulo();
	}


	//Styrsignal OCB styr hela tiden benet brevid OCA
	//men de styr samma servonummer
	if(uppdrag == 1 && servo == 2)
	{
		if(benA == 3)
		{
			OCR1A = pgm_read_word(&styrmatris[position][benA][servo]) + kompensation[riktning] + 40*reglering;
			OCR1B = pgm_read_word(&styrmatris[position][benB][servo]) + kompensation[riktning] + 40*reglering;
		}
		else if(benA == 4)
		{ 
			OCR1A = pgm_read_word(&styrmatris[position][benA][servo]) + kompensation[riktning] + 40*reglering;
			OCR1B = pgm_read_word(&styrmatris[position][benB][servo]);
		}
		else if(benB == 3)
		{
			OCR1A = pgm_read_word(&styrmatris[position][benA][servo]);
			OCR1B = pgm_read_word(&styrmatris[position][benB][servo]) + kompensation[riktning] + 40*reglering;
		}
		else if(benA == 0)
		{
			OCR1A = pgm_read_word(&styrmatris[position][benA][servo]) - kompensation[riktning] - 40*reglering;
			OCR1B = pgm_read_word(&styrmatris[position][benB][servo]) - kompensation[riktning] - 40*reglering;
		}
		else if(benA == 1)
		{ 
			OCR1A = pgm_read_word(&styrmatris[position][benA][servo]) - kompensation[riktning] - 40*reglering;
			OCR1B = pgm_read_word(&styrmatris[position][benB][servo]);
		}
		else if(benB == 0)
		{
			OCR1A = pgm_read_word(&styrmatris[position][benA][servo]);
			OCR1B = pgm_read_word(&styrmatris[position][benB][servo]) - kompensation[riktning] - 40*reglering;
		}							
	}
	else
	{
		OCR1A = pgm_read_word(&styrmatris[position][benA][servo]); 
		OCR1B = pgm_read_word(&styrmatris[position][benB][servo]);
	}


//Finjustering f�r enskillda ben. Framtid...	
/*
	if(benA == 1 && servo == 1)
	{
		OCR1A = pgm_read_word(&styrmatris[position][benA][servo] + 100);
	}
	else if(benB == 1 && servo == 1)
	{
		OCR1B = pgm_read_word(&styrmatris[position][benB][servo] + 100);
	}
*/

	//Uppdaterar styrsignal n�r den skickats i ca 0.5 sek
	if(varv == varv_tak)
	{
		//Stanna / St� still grundl�ge
		if(uppdrag == 0)
		{
			movement_complete = 1;
			//position = 0;
			varv = 1;
		}
		//G� 
		else if(uppdrag == 1)
		{
			movement_complete = 0;
			varv_tak = 9*3;
			//Byt till n�sta position
			position++;
			if(position > 8)
			{
				position = 3;

/*
				steps++;
				if(steps > 6)
				{
					uppdrag = 0;
					position = 0;
					steps = 0;


				}
*/
			}
			varv = 1;
		}
		//ROTERA Medurs
		else if(uppdrag == 2)
		{
			movement_complete = 0;
			// S�tter rotationshastigheten
			varv_tak = 9*20;

			//Byt till n�sta position
			position++;
			if(position > 17)
			{
				position = 0;
				uppdrag = 0;

				riktning += 5;
				temp_riktning += 5;
				if(riktning > 5)
				{
					riktning -= 6;
				}
				if(temp_riktning > 5)
				{
					temp_riktning -= 6;
				}
				
				uppdatera_ben_mux();
			}
			varv = 1;
		}
		//ROTERA Moturs
		else if(uppdrag == 3) 
		{
			movement_complete = 0;
			// S�tter rotationshastigheten
			varv_tak = 9*20;

			//Byt till n�sta position
			position++;
			if(position > 26)
			{
				position = 0;				
				uppdrag = 0;
				riktning += 1;
				temp_riktning += 1;
				if(riktning > 5)
				{
					riktning -= 6;
				}
				if(temp_riktning > 5)
				{
					temp_riktning -= 6;
				}

				uppdatera_ben_mux();
					
			}
			varv = 1;
		}
		//Rotera medurs LITE
		else if(uppdrag == 4)
		{
			movement_complete = 0;
			//Byt till n�sta position
			position++;
			if(position > 33)
			{
				position = 27;
			}
			varv = 1;
		}
		//Rotera moturs LITE
		else if(uppdrag == 5)
		{
			movement_complete = 0;
			//Byt till n�sta position
			position++;
			if(position > 40)
			{
				position = 34;
			}
			varv = 1;
		}
			
		else
		{
			varv = 1;
		}
	}
	varv++;
}


//------------------------------------
//		TWI_SLAVE-KOD
//------------------------------------

//Initiering, antal bytes mottagna eller skickade
unsigned char bytes = 0;

//Initiering av TWI:n
void TWIslave_init()
{
	TWAR = (SLAVE_ADRESS<<1);
	//st�ll in TWI-srreg s� att den skickar ack n�r den addreseras
	TWCR = (1<<TWEN)|(1<<TWIE)|(0<<TWINT)|
		   (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|
           (0<<TWWC);
}

//	KOMMANDOTOLKNING
void interpret_data()
{
	//F�rsta byten, data[0] skall splittas i tv� minibytes
	char datatyp = data[0] >> 4;
	char sensor_reglering = data[1];

	riktning = temp_riktning;
	// Kod som ska tolka kommandon f�r att best�mma uppdrag
	switch(datatyp)
	{
		//Best�m riktning manuellt
		case 0:
			//set_riktning(rikt_arg);
			break;

		//G� fram�t
		case 2:
			temp_riktning = riktning;
			uppdatera_ben_mux();
			uppdrag = 1;
			position = 3;
			break;

		//G� bak�t
		case 3:
			temp_riktning = riktning;
			riktning += 3;
			if (riktning > 5)
			{
				riktning -= 6;
			}
			
			uppdatera_ben_mux();
			uppdrag = 1;
			position = 3;
			break;

		//Rotera v�nster
		case 4:
			uppdrag = 3;
			position = 18;
			break;

		//Rotera h�ger
		case 5:
			uppdrag = 2;
			position = 9;
			break;

		//Stanna
		case 6:
			uppdrag = 0;
			position = 0;
			break;

		//Skicka movement_complete
		case 7:
			//Sker nu automatiskt d� det �r det �nda som skickas

			break;
			
		default:
			//Hamnar man h�r har man skickat ett d�ligt kommando
			//Eventuellt skiter man i det och forts�tter eller 
			//s� stannar man
			break;
	}

	// Hantera regleringen ifr�n sensorenheten
	switch(sensor_reglering)
	{
		//Ingen reglering
		case 0:
			reglering = 0;
			break;
		
		//reglera H�ger
		case 1:
			reglering = 1;
			break;
		
		//reglera V�nster
		case 2:
			reglering = -1;
			break;
	}


/*	// Hantera regleringen ifr�n sensorenheten som �r relativ felet
	if(sensor_reglering != 0)
	{
		// Om MSB �r satt s� �r talet negativt
		if(sensor_reglering & 0xF0)
		{
			relgering = -sensor_reglering;
		}
		else
		{
			reglering = sensor_reglering;
		}
	}
	else
	{
		relgering = 0;
	}
*/
}

//AVBROTTSRUTIN
//som aktiveras av kommunikationsenheten
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
			TWCR &= ~(1<<TWSTO);                
			break;
	
		//SLA+W mottagen, ACK skickad, arbitration lost i master
		case TW_SR_ARB_LOST_SLA_ACK:
	
		//om ej data vill s�ndas
	
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

		//Stop eller upprepad start n�r man �r addreserad
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
			TWDR = movement_complete;
			TWCR &= ~(1<<TWSTO);
			TWCR |= (1<<TWINT)|(1<<TWEA); 
	
			bytes += 1;               
		break;
			//Data skickad, ACK mottagen
		case TW_ST_DATA_ACK:
			//Skicka data och tag emot ack
			if(bytes < MAX_DATA_BYTES - 1)
			{
				TWDR = 0;
				TWCR &= ~(1<<TWSTO);
				TWCR |= (1<<TWINT)|(1<<TWEA);
				
				bytes += 1;
			}
			else
			{
 				TWDR = 0;
				TWCR &= ~((1<<TWSTO) | (1<<TWEA));
				TWCR |= (1<<TWINT);
				bytes = 0;
				
			}
		break;

		case TW_ST_LAST_DATA:
			//Tag emot framtida meddelanden
			TWCR |= (1<<TWINT)|(1<<TWEA);
			TWCR &= ~(1<<TWSTO);
	
		break;

		default:
 			TWCR &= ~((1<<TWSTO)|(1<<TWEA));
			TWCR |= (1<<TWINT);
			
		break;
	}

}



int main()
{
	// S�tter register och liknande
	styr_setup();
	// F�rslavar styrenheten
	TWIslave_init();
	sei();

 	DDRA = 0xFF;

	// Avbrotten ovan kommer att aktiveras i denna loop
	while(1){};


	return 0;
}
