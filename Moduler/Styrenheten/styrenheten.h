#ifndef STYRENHETEN_H
#define STYRENHETEN_H

unsigned char movement_complete;

void twi_slave_init();
void set_riktning(char rikt_arg);
void ben_modulo();
void uppdatera_ben_mux();
void styr_setup(void);
ISR(TIMER1_OVF_vect);
ISR(TWI_vect);
int main();


#endif
