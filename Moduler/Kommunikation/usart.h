#ifndef USART_H
#define USART_H

#define BAUD 115200
#define FOSC 14745600
#define MYUBRR 15//FOSC/16/BAUD-1

#define start_datatyp 0b10010000
#define regler_parameter_datatyp 0b00000000
#define karta_datatyp 0b00010000
#define framatriktning_datatyp 0b00100000
#define position_datatyp 0b00110000
#define ga_i_riktning_datatyp 0b01000000
#define rotera_datatyp 0b01010000
#define stanna_datatyp 0b01100000
#define sensor_datatyp 0b01110000
#define slut_datatyp 0b10000000

#define BUFFER_SIZE 50

uint8_t buffer_data[6];
uint8_t mottagen_data[BUFFER_SIZE];   // 0 på alla element
uint8_t mottagendata_ledig;
uint8_t regler_data;
uint8_t kart_data;
uint8_t framatriktning_data;
uint8_t position_data;
uint8_t ga_i_riktning;
uint8_t sensor0_data;
uint8_t sensor1_data;
uint8_t sensor2_data;
uint8_t sensor3_data;
uint8_t sensor4_data;
uint8_t sensor5_data;
int sandning_typ;


void USART_Init (); 
ISR(USART_RXC_vect);
void USART_Transmit(uint8_t send_data[], uint8_t, uint8_t);
//int main();

#endif
