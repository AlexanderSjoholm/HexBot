#include <styrmatris.h>
#include <avr/pgmspace.h>

//********STYRMATRIS*************

// Matrisparametrar

	//Servol�gen:

	//Definition av respektive servos h�g-, mellan- och l�g- l�ge.
	//Varje l�ge multipliceras med 9 f�r att duty cycle 
	//ska st�mma med den totala periodtiden
	//top*(1 - duty cycle*9)

	//Servo 1. Kn�led
	#define l1 130
	#define m1 100
	#define h1 30	//61

	//servo 2. Fotled
	#define l2 135
	#define m2 190

	//Servo 3. H�ftled
	#define l3 380//441
	#define h3 320//341

	//Speciall�gen:

	//H�ftleden beh�ver tv� l�gen f�r att kunna peka "raktfram"
	//f/b - fr�mre/bakre, r - raktfram, l/r - left/righ, 3 - servo 3.
	//Reglering, positiva v�rden motsvarar h�ger och vice versa
	#define frl3 515 //541 // top*(1 - 0.057*9)
	#define frr3 215 //241 // top*(1 - 0.087*9)
	#define brl3 515 //541 // top*(1 - 0.057*9) 
	#define brr3 215 //241 // top*(1 - 0.087*9)

	//Sidobenen
	#define sm1 100 // top*(1 - 0.089*9)
	#define sh1 160 // top*(1 - 0.089*9)
	#define sl2 135 // top*(1 - 0.091*9)

	//Center, Det l�ge d� servot ska st� helt centrerat
	#define c 350//391 // top*(1 - 0.072*9)
  	
	// Anv�nds f�r rotation 15�.
	#define med15 238 // top*(1 - 0.086*9)
	#define mot15 594 // top*(1 - 0.053*9)

	//F�r mycket liten rotation
	#define medr 180	//brr3 	215
	#define medl 480	//frl3 	515
	#define meds 306	//h3 	341

	#define motr 250	//brr3 	215
	#define motl 550	//frl3 	515
	#define mots 376	//h3 	341




//STYRMATRIS. D�r �r H�R man s�tter alla positioner och all skit.
//styrsignaler f�r respektive position, ben och servo
//styrmatris[position][ben][servo]

//POSITIONSINDEX
//Position 0-2: ???
//Position 3-8: G�
//Position 9-26: Rotera

const int styrmatris[45][6][3] PROGMEM = {
	//------------------------------
	//		BASIC
	//------------------------------
	//Position0 - 
	{	
		{l1,m2,brl3},
		{m1,l2,brr3},
		{sm1,sl2,h3},
		{l1,m2,frl3},
		{m1,l2,frr3},
		{sm1,sl2,h3}
	},
	//Tv� skitl�gen utan funktion
	//Position1
	{
		{m1,l2,c},
		{m1,l2,c},
		{m1,l2,c},
		{m1,l2,c},
		{m1,l2,c},
		{m1,l2,c}
	},
	//position2
	{
		{c,c,c},
		{c,c,c},
		{c,c,c},
		{c,c,c},
		{c,c,c},
		{c,c,c}
	},
	//------------------------------
	//		G�
	//------------------------------
	//position3 - Grundl�ge
	{
		{l1,m2,brl3},
		{m1,l2,brr3},
		{sm1,sl2,h3},
		{l1,m2,frl3},
		{m1,l2,frr3},
		{sm1,sl2,h3}
	},
	//position4 - Lyft udda
	{
		{h1,m2,brl3},
		{m1,l2,brr3},
		{h1,sl2,h3},
		{l1,m2,frl3},
		{h1,l2,frr3},
		{sm1,sl2,h3}
	},
	//position5 - G�
	{
		{h1,l2,brl3},
		{l1,m2,brr3},
		{h1,sl2,l3},
		{m1,l2,frl3},
		{h1,m2,frr3},
		{sm1,sl2,l3}
	},
	//position6 - St�ll ner
	{
		{m1,l2,brl3},
		{l1,m2,brr3},
		{sm1,sl2,l3},
		{m1,l2,frl3},
		{l1,m2,frr3},
		{sm1,sl2,l3}
	},
	//position7 - Lyft j�mn
	{
		{m1,l2,brl3},
		{h1,m2,brr3},
		{sm1,sl2,l3},
		{h1,l2,frl3},
		{l1,m2,frr3},
		{h1,sl2,l3}
	},
	//position8 G�
	{
		{l1,m2,brl3},
		{h1,l2,brr3},
		{sm1,sl2,h3},
		{h1,m2,frl3},
		{m1,l2,frr3},
		{h1,sl2,h3}
	},
	//------------------------------
	//		ROTERA Medurs 
	//------------------------------
		//position9 Grundl�ge - samma som vid g�ng
	{
		{l1,m2,brl3},
		{m1,l2,brr3},
		{sm1,sl2,h3},
		{l1,m2,frl3},
		{m1,l2,frr3},
		{sm1,sl2,h3}
	},	
	//position10 - lyft udda ben
	{
		{h1,m2,brl3},
		{m1,l2,brr3},
		{h1,sl2,h3},
		{l1,m2,frl3},
		{h1,l2,frr3},
		{sm1,sl2,h3}
	},
	//position11 - rotera luftbenen
	{
		{h1,l2,mot15},
		{m1,l2,brr3},
		{h1,sl2,mot15},
		{l1,m2,frl3},
		{h1,l2,mot15},
		{sm1,sl2,h3}
	},
	//position12 - st�ll ner benen
	{
		{l1,l2,mot15},
		{m1,l2,brr3},
		{sm1,sl2,mot15},
		{l1,m2,frl3},
		{l1,l2,mot15},
		{sm1,sl2,h3}
	},
	//position13 - lyft j�mna ben
	{
		{l1,l2,mot15},
		{h1,l2,brr3},
		{sm1,sl2,mot15},
		{h1,m2,frl3},
		{l1,l2,mot15},
		{h1,sl2,h3}
	},
	//position14 - rotera markbenen, st�ll �vriga ben r�tt fast i luften
	{
		{l1,l2,med15},
		{h1,m2,brl3},
		{sm1,sl2,med15},
		{h1,sl2,h3},
		{l1,l2,med15},
		{h1,l2,frr3}
	},
	//position15 - st�ll ner j�mna ben
	{
		{l1,l2,med15},
		{l1,m2,brl3},
		{sm1,sl2,med15},
		{sm1,sl2,h3},
		{l1,l2,med15},
		{m1,l2,frr3}
	},
	//position16 - lyft udda ben
	{
		{h1,l2,med15},
		{l1,m2,brl3},
		{h1,sl2,med15},
		{sm1,sl2,h3},
		{h1,l2,med15},
		{m1,l2,frr3}
	},
	//position17 - st�ll i r�tt l�ge i luften
	{
		{h1,sl2,h3},
		{l1,m2,brl3},
		{h1,l2,brr3},
		{sm1,sl2,h3},
		{h1,m2,frl3},
		{m1,l2,frr3}
	},
	//------------------------------
	//		ROTERA Moturs 
	//------------------------------
	//position18 Grundl�ge - samma som vid g�ng
	{
		{l1,m2,brl3},
		{m1,l2,brr3},
		{sm1,sl2,h3},
		{l1,m2,frl3},
		{m1,l2,frr3},
		{sm1,sl2,h3}
	},	
	//position19 - lyft udda ben
	{
		{h1,m2,brl3},
		{m1,l2,brr3},
		{h1,sl2,h3},
		{l1,m2,frl3},
		{h1,l2,frr3},
		{sm1,sl2,h3}
	},
	//position20 - rotera luftbenen
	{
		{h1,l2,med15},
		{m1,l2,brr3},
		{h1,sl2,med15},
		{l1,m2,frl3},
		{h1,l2,med15},
		{sm1,sl2,h3}
	},
	//position21 - st�ll ner benen
	{
		{l1,l2,med15},
		{m1,l2,brr3},
		{sm1,sl2,med15},
		{l1,m2,frl3},
		{l1,l2,med15},
		{sm1,sl2,h3}
	},
	//position22 - lyft j�mna ben
	{
		{l1,l2,med15},
		{h1,l2,brr3},
		{sm1,sl2,med15},
		{h1,m2,frl3},
		{l1,l2,med15},
		{h1,sl2,h3}
	},
	//position23 - rotera markbenen, st�ll �vriga ben r�tt fast i luften
	{
		{l1,l2,mot15},
		{h1,sl2,h3},
		{sm1,sl2,mot15},
		{h1,l2,frr3},
		{l1,l2,mot15},
		{h1,m2,brl3}
	},
	//position24 - st�ll ner j�mna ben
	{
		{l1,l2,mot15},
		{sm1,sl2,h3},
		{sm1,sl2,mot15},
		{m1,l2,frr3},
		{l1,l2,mot15},
		{l1,m2,brl3}
	},
	//position25 - lyft udda ben
	{
		{h1,l2,mot15},
		{sm1,sl2,h3},
		{h1,sl2,mot15},
		{m1,l2,frr3},
		{h1,l2,mot15},
		{l1,m2,brl3}
	},
	//position26 - st�ll i r�tt l�ge i luften
	{
		{h1,l2,brr3},
		{sm1,sl2,h3},
		{h1,m2,frl3},
		{m1,l2,frr3},
		{h1,sl2,h3},
		{l1,m2,brl3}
	},
	//------------------------------
	//		ROTERA Medurs LITE!
	//------------------------------
	//position27 Grundl�ge - samma som vid g�ng
	{
		{l1,m2,brl3},
		{m1,l2,brr3},
		{sm1,sl2,h3},
		{l1,m2,frl3},
		{m1,l2,frr3},
		{sm1,sl2,h3}
	},	
	//position28 - lyft j�mna ben
	{
		{h1,m2,brl3},
		{m1,l2,brr3},
		{h1,sl2,h3},
		{l1,m2,frl3},
		{h1,l2,frr3},
		{sm1,sl2,h3}
	},
	//position29 - rotera markben lite lite..
	{
		{h1,m2,brl3},
		{m1,l2,medr},
		{h1,sl2,h3},
		{l1,m2,medl},
		{h1,l2,frr3},
		{sm1,sl2,meds}
	},
	//position30 - st�ll ner benen
	{
		{l1,m2,brl3},
		{m1,l2,medr},
		{sm1,sl2,h3},
		{l1,m2,medl},
		{m1,l2,frr3},
		{sm1,sl2,meds}
	},
	//position31 - lyft udda ben
	{
		{l1,m2,brl3},
		{h1,l2,medr},
		{sm1,sl2,h3},
		{h1,m2,medl},
		{m1,l2,frr3},
		{h1,sl2,meds}
	},
	//position32 - st�ll ben r�tt i luften
	{
		{l1,m2,brl3},
		{h1,l2,brr3},
		{sm1,sl2,h3},
		{h1,m2,frl3},
		{m1,l2,frr3},
		{h1,sl2,h3}
	},
	//position33 - st�ll ner j�mna ben
	{
		{l1,m2,brl3},
		{m1,l2,brr3},
		{sm1,sl2,h3},
		{l1,m2,frl3},
		{m1,l2,frr3},
		{sm1,sl2,h3}
	},
	//-------------------------------
	//		ROTERA Moturs LITE!!
	//-------------------------------
	//position34 Grundl�ge - samma som vid g�ng
	{
		{l1,m2,brl3},
		{m1,l2,brr3},
		{sm1,sl2,h3},
		{l1,m2,frl3},
		{m1,l2,frr3},
		{sm1,sl2,h3}
	},	
	//position35 - lyft j�mna ben
	{
		{h1,m2,brl3},
		{m1,l2,brr3},
		{h1,sl2,h3},
		{l1,m2,frl3},
		{h1,l2,frr3},
		{sm1,sl2,h3}
	},
	//position36 - rotera markben lite lite..
	{
		{h1,m2,brl3},
		{m1,l2,motr},
		{h1,sl2,h3},
		{l1,m2,motl},
		{h1,l2,frr3},
		{sm1,sl2,mots}
	},
	//position37 - st�ll ner benen
	{
		{l1,m2,brl3},
		{m1,l2,motr},
		{sm1,sl2,h3},
		{l1,m2,motl},
		{m1,l2,frr3},
		{sm1,sl2,mots}
	},
	//position38 - lyft udda ben
	{
		{l1,m2,brl3},
		{h1,l2,motr},
		{sm1,sl2,h3},
		{h1,m2,motl},
		{m1,l2,frr3},
		{h1,sl2,mots}
	},
	//position39 - st�ll ben r�tt i luften
	{
		{l1,m2,brl3},
		{h1,l2,brr3},
		{sm1,sl2,h3},
		{h1,m2,frl3},
		{m1,l2,frr3},
		{h1,sl2,h3}
	},
	//position40 - st�ll ner j�mna ben
	{
		{l1,m2,brl3},
		{m1,l2,brr3},
		{sm1,sl2,h3},
		{l1,m2,frl3},
		{m1,l2,frr3},
		{sm1,sl2,h3}
	},
};
