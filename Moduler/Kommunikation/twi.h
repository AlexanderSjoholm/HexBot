#ifndef TWI_H
#define TWI_H


//Ofta anv�nda v�rden som skickas f�r att starta, sluta och 
//Skicka data p� bussen
#define I2C_START 0	
#define I2C_DATA  1
#define I2C_STOP  2

//Max skickaningsf�rs�k innan den d�r
#define MAX_TRIES 5
#define MAX_DATA_BYTES 6

#define RECEIVE_SENSOR_DATA 0x01
#define RECEIVE_MAP_DATA 0x02
#define RECEIVE_REGULATOR_DATA 0x03
#define RECEIVE_MOVEMENT_DATA 0x04

extern unsigned char data[MAX_DATA_BYTES];

void twi_init();
unsigned char i2c_transmit(unsigned char type);
int i2c_writebyte(unsigned char adress);
int i2c_readbyte(unsigned char adress);


#endif
