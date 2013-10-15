#ifndef TWI_SLAVE_H
#define TWI_SLAVE_H

//Slavens adress
#define SLAVE_ADRESS 0x02
//maxantalet bytes som kan tas emot
#define MAX_DATA_BYTES 6
//char-array med data som skall tas emot
char data[MAX_DATA_BYTES];
unsigned char bytes;

#define SEND_SENSOR_DATA 0x01
#define SEND_MAP_DATA 0x02
#define SEND_REGULATOR_DATA 0x03
#define SEND_MOVEMENT_DATA 0x04

#define MOVEMENT_COMPLETE 0x05


void TWIslave_init();
void interpret_data();
ISR(TWI_vect);


#endif
