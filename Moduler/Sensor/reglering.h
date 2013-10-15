#ifndef REGLERING_H
#define REGLERING_H

unsigned char regulation_mode;
//unsigned char direction; Här skall väl den extern direction variabel som finns i adc användas

unsigned char get_local_wall(unsigned char x, unsigned char y);
void set_local_wall(unsigned char wall, unsigned char pos);
unsigned char get_local_searched(unsigned char x, unsigned char y);
void set_local_searched(unsigned char wall, unsigned char pos);
extern unsigned char movement_data[6]; //Behövs väl för att använda data..?

unsigned char modulo_6(unsigned char value);

void look_for_front_wall(unsigned char front);
void look_for_left_wall(unsigned char left_1, unsigned char left_2);
void look_for_back_wall(unsigned char back);
void look_for_right_wall(unsigned char right_1, unsigned char right_2);
void look_for_walls();

void reglera();
void start_up_routine();
void movement_direction();


#endif
