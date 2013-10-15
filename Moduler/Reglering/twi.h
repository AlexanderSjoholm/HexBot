#ifndef TWI_H
#define TWI_H


//Ofta anv�nda v�rden som skickas f�r att starta, sluta och 
//Skicka data p� bussen
#define I2C_START 0	
#define I2C_DATA  1
#define I2C_STOP  2

//Max skickaningsf�rs�k innan den d�r
#define MAX_TRIES 100
#define MAX_DATA_BYTES 6

void twi_init();
unsigned char i2c_transmit(unsigned char type);
int i2c_writebyte(unsigned char adress, char data[MAX_DATA_BYTES]);
int i2c_readbyte(unsigned char adress, char * data[MAX_DATA_BYTES]);
int main();

#endif
