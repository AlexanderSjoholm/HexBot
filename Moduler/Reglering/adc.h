#ifndef ADC_H
#define ADC_H

#define no_mode 0x00
#define reg_mode 0x01
#define map_mode 0x02
#define read_mode 0x03

#define max_number_reads 4

//Avstånd till de väggar som är parallella med robotens färdriktning
#define side_wall_dist_1 0x50
#define side_wall_dist_2 0x50 //Detta värde stämmer inte

//Avstånd till de väggar som är vinkelräta mot robotens färdriktning
#define wall_dist_1 0x59
#define wall_dist_2 0x50 //Detta värde stämmer inte
#define wall_dist_3 0x50 //Detta värde stämmer inte

unsigned char adc_ch;
volatile unsigned char mode;
extern unsigned char measurements[6][4];
unsigned char reads;
unsigned char mesurement;

extern unsigned char side_wall;
extern unsigned char direction;


void ADCinit();
void read_sensors();
unsigned char get_measurements(unsigned char, unsigned char);
void set_measurements(unsigned char, unsigned char, unsigned char);
unsigned char decide_side_wall();
//ISR(ADC_vect);

#endif
