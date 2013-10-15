#ifndef TWI_SLAVE_H
#define TWI_SLAVE_H

//Slavens adress
#define SLAVE_ADRESS 0x02
//maxantalet bytes som kan tas emot
#define MAX_DATA_BYTES 6
//char-array med data som skall tas emot
volatile unsigned char data[MAX_DATA_BYTES];
unsigned char from_sensor_data[MAX_DATA_BYTES];
volatile unsigned char bytes;

#define SEND_SENSOR_DATA 0x01
#define READ_SENSOR_DATA 0x10

#define SEND_MAP_DATA 0x02
#define SEND_REGULATOR_DATA 0x03

unsigned char get_send_data(unsigned char);
void set_send_data(unsigned char, unsigned char);

void TWIslave_init();
void interpret_data();
ISR(TWI_vect);


#endif
