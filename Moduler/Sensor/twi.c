#include <avr/io.h>
#include <compat/twi.h>
#include <avr/interrupt.h>
#include <twi.h>


void twi_init()
{
	TWBR |= 0x02; //Set bitrate
}

unsigned char i2c_transmit(unsigned char type) {
  switch(type) {
     case I2C_START:    // Send Start Condition
       TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
       break;
     case I2C_DATA:     // Send Data
       TWCR = (1 << TWINT) | (1 << TWEN);
       break;
     case I2C_STOP:     // Send Stop Condition
       TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
       return 0;
  }  
  
  // Wait for TWINT flag set in TWCR Register
  while (!(TWCR & (1 << TWINT)));  
  
  // Return TWI Status Register, mask the prescaler bits (TWPS1,TWPS0)
  return (TWSR & 0xF8);
}

int i2c_writebyte(unsigned char adress, char data[MAX_DATA_BYTES])
{
//Antal försök
  unsigned char n = 0;
  unsigned char bytes = 0;
  unsigned char twi_status;
//Retur om allt skiter sig
  char r_val = -1;
  
  i2c_retry:
  	if (n++ >= MAX_TRIES) return r_val;  
  	
	bytes = 0;
  	// Transmit Start Condition
 	twi_status = i2c_transmit(I2C_START);
 
	// Check the TWI Status
	if (twi_status == TW_MT_ARB_LOST) goto i2c_retry;
	if ((twi_status != TW_START) && (twi_status != TW_REP_START)) goto i2c_quit;  
	
	//Send slave address (SLA_W)  
	//Här anger vi någon av de två adresser som vi kommer att ha.
	//Antingen Sensor eller Styr
	//Här anges en adress. Kanske adress ett: 0b00000001. 
	//Denna skiftas ett steg för att inte skriv biten 
	//ska skriva över adressen
	TWDR = (adress << 1) | TW_WRITE;  

	// Transmit I2C Data. Adressen skickas varpå ett statusmeddelande returneras
	twi_status = i2c_transmit(I2C_DATA);  

	// Check the TWSR status
	if ((twi_status == TW_MT_SLA_NACK) || (twi_status == TW_MT_ARB_LOST)) goto i2c_retry;
	if (twi_status != TW_MT_SLA_ACK) goto i2c_quit; 
	
	
	   

  i2c_next_data_byte:
  	
	if(bytes < MAX_DATA_BYTES)
	{
		// Put data into data register and start transmission
		//Nu fylls registret med det man vill skicka
		TWDR = data[bytes];  
	
		// Transmit I2C Data
		twi_status = i2c_transmit(I2C_DATA);  
		bytes += 1;

		if (twi_status != TW_MT_DATA_ACK) goto i2c_retry;

		goto i2c_next_data_byte;
	}
	
	// TWI Transmit Ok
	r_val = 1;
	
  i2c_quit:
  
	// Transmit I2C Data
	twi_status = i2c_transmit(I2C_STOP);
  
  return r_val;
}


int i2c_readbyte(unsigned char adress, char * data[MAX_DATA_BYTES])
{
//Antal försök
  unsigned char n = 0;
  unsigned char bytes = 0;
  unsigned char twi_status;
  char r_val = -1;
  

  i2c_retry:

	if (n++ >= MAX_TRIES) return r_val;  
  	
	bytes = 0;
	// Transmit Start Condition
	twi_status = i2c_transmit(I2C_START);

	// Check the TWSR status
	if (twi_status == TW_MT_ARB_LOST) goto i2c_retry;
	if ((twi_status != TW_START) && (twi_status != TW_REP_START)) goto i2c_quit;    
  
	// Send slave address (SLA_R)
	TWDR = (adress << 1) | TW_READ;  
  
	// Transmit I2C Data
	twi_status = i2c_transmit(I2C_DATA);  

	// Check the TWSR status
	if ((twi_status == TW_MR_SLA_NACK) || (twi_status == TW_MR_ARB_LOST)) goto i2c_retry;
	if (twi_status != TW_MR_SLA_ACK) goto i2c_quit;  



  i2c_next_data_byte:
  	if (twi_status != TW_MR_DATA_ACK) goto i2c_retry;
	if(bytes < MAX_DATA_BYTES)
	{
		// Put data into data register and start transmission
		//Nu fylls registret med det man vill skicka
		(*data)[bytes] = TWDR;  
	
		// Transmit I2C Data
		twi_status = i2c_transmit(I2C_DATA);  
		bytes += 1;
		goto i2c_next_data_byte;
	}
	
	r_val = 1;
	  
  i2c_quit:
  
	// Send Stop Condition
	twi_status = i2c_transmit(I2C_STOP);
	return r_val;
}


int main()
{
	DDRA = 0xFF;
	twi_init();	

	for(;;)
	{
	for(int i = 0; i < 1000; i++)
	{
	char data[6] = {1,2,3,4,5,6};
	i2c_writebyte(0x02, data);
	}
	}
return 0;
}

