#ifndef TWI_SLAV_H
#define TWI_SLAV_H

//Slavens adress
#define SLAVE_ADRESS 0x01
//maxantalet bytes som kan tas emot
#define MAX_DATA_BYTES 6
//char-array med data som skall tas emot
char data[MAX_DATA_BYTES];
unsigned char bytes;


void TWIslave_init();
void interpret_data();
ISR(TWI_vect);


#endif
