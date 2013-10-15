#ifndef USART_H
#define USART_H
/*
#include <avr/interrupt.h>
#include <avr/io.h>
*/
#define BAUD 115200
#define FOSC 14745600
#define MYUBRR 15//FOSC/16/BAUD-1
//#define nop()__asm__ __volatile__ ("nop")

void USART_Init (); 
ISR(USART_RXC_vect);
ISR(USART_TXC_vect);
void USART_Transmit(unsigned char);
//int main();

#endif
