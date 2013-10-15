#ifndef TWI_H
#define TWI_H


//Ofta använda värden som skickas för att starta, sluta och 
//Skicka data på bussen
#define I2C_START 0	
#define I2C_DATA  1
#define I2C_STOP  2

//Max skickaningsförsök innan den dör
#define MAX_TRIES 100
#define MAX_DATA_BYTES 6

void twi_init();
unsigned char i2c_transmit(unsigned char type);
int i2c_writebyte(unsigned char adress, char data[MAX_DATA_BYTES]);
int i2c_readbyte(unsigned char adress, char * data[MAX_DATA_BYTES]);
int main();

#endif
