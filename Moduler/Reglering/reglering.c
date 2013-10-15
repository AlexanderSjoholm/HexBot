#include <adc.h>
#include <reglering.h>

//De olika reglertillst�nd som existerar
#define manuell 0
#define start_up 1
#define wall_l 0b1000
#define wall_f 0b0100
#define wall_r 0b0010
#define wall_b 0b0001


//Den till�tna marginal f�r v�gg detektering. Skall testas
#define marginal 10

//Variabel som ettst�lls n�r roboten f�rs�krat sig om att den st�r parallellt
//med en v�gg. Denna ska nollst�llas n�r roboten t ex passaerar ett h�rn och 
//inte l�ngre har b�da sensorerna mot samma v�gg.

unsigned char regulation_mode = manuell;
//Variabel som definierar n�gon form av global riktning 0-3 (v�nster, fram, h�ger, bak)
//Denna uppdateras vid varje rotation och riktningsbyte
unsigned char global_north = 0;
unsigned char local_walls[3][3];
unsigned char local_searched[3][3];

//De styrdata som ska skickas till styrenheten
unsigned char movement_data[6];


unsigned char get_local_wall(unsigned char x, unsigned char y)
{
	return local_walls[x][y];
}

void set_local_wall(unsigned char wall, unsigned char pos)
{
	unsigned char x = (pos & 0xF0) >> 4;
	unsigned char y = (pos & 0x0F);

	unsigned char temp = get_local_wall(x,y);
	temp |= wall;
	local_walls[x][y] = temp;
}

unsigned char get_local_searched(unsigned char x, unsigned char y)
{
	return local_searched[x][y];
}

void set_local_searched(unsigned char wall, unsigned char pos)
{
	unsigned char x = (pos & 0xF0) >> 4;
	unsigned char y = (pos & 0x0F);

	unsigned char temp = get_local_searched(x,y);
	temp |= wall;
	local_searched[x][y] = temp;
}

unsigned char modulo_6(unsigned char value)
{
	if (value > 5)
	{
		value -= 6;
	}
	return value;
}

void look_for_front_wall(unsigned char front)
{
	if((front < (wall_dist_1 + marginal)) && (front > (wall_dist_1 - marginal)))
	{
		set_local_wall(wall_f,0x11);
		set_local_searched(wall_f,0x11);
	}
	else if((front < (wall_dist_2 + marginal)) && (front > (wall_dist_2 - marginal)))
	{
		set_local_wall(wall_f,0x12);
		set_local_searched(wall_f,0x11);
		set_local_searched(wall_f,0x12);
	}
	else
	{
		set_local_searched(wall_f,0x11);
		set_local_searched(wall_f,0x12);
	}
}

void look_for_left_wall(unsigned char left_1, unsigned char left_2)
{
	if(((left_1 < (side_wall_dist_1 + marginal)) && (left_1 > (side_wall_dist_1 - marginal)))
	&& ((left_2 < (side_wall_dist_1 + marginal)) && (left_2 > (side_wall_dist_1 - marginal))))
	{
		set_local_wall(wall_l,0x11);
		set_local_searched(wall_l,0x11);
	}
	else
	{
		if((left_1 < (wall_dist_3 + marginal)) && (left_1 > (wall_dist_3 - marginal)))
		{
			set_local_wall(wall_f,0x01);
			set_local_searched(wall_f,0x01);
			set_local_searched(wall_l,0x11);
		}
		else if((left_1 < (side_wall_dist_2 + marginal)) && (left_1 > (side_wall_dist_2 - marginal)))
		{
			set_local_wall(wall_l,0x02);
			set_local_searched(wall_l,0x02);
			set_local_searched(wall_f,0x01);
			set_local_searched(wall_l,0x11);
		}
		else
		{
			set_local_searched(wall_l,0x02);
			set_local_searched(wall_f,0x01);
			set_local_searched(wall_l,0x11);
		}
				
		if((left_2 < (wall_dist_3 + marginal)) && (left_2 > (wall_dist_3 - marginal)))
		{
			set_local_wall(wall_b,0x01);
			set_local_searched(wall_b,0x01);
			set_local_searched(wall_l,0x11);
		}
		else if((left_2 < (side_wall_dist_2 + marginal)) && (left_2 > (side_wall_dist_2 - marginal)))
		{
			set_local_wall(wall_l,0x00);
			set_local_searched(wall_l,0x00);
			set_local_searched(wall_b,0x01);
			set_local_searched(wall_l,0x11);
		}
		else
		{
			set_local_searched(wall_l,0x00);
			set_local_searched(wall_b,0x01);
			set_local_searched(wall_l,0x11);
		}
	}
}

void look_for_back_wall(unsigned char back)
{
	if((back < (wall_dist_1 + marginal)) && (back > (wall_dist_1 - marginal)))
	{
		set_local_wall(wall_b,0x11);
		set_local_searched(wall_b,0x11);
	}
	else if((back < (wall_dist_2 + marginal)) && (back > (wall_dist_2 - marginal)))
	{
		set_local_wall(wall_b,0x10);
		set_local_searched(wall_b,0x10);
		set_local_searched(wall_b,0x11);
	}
 }

void look_for_right_wall(unsigned char right_1, unsigned char right_2)
{

	if(((right_1 < (side_wall_dist_1 + marginal)) && (right_1 > (side_wall_dist_1 - marginal)))
	&& ((right_2 < (side_wall_dist_1 + marginal)) && (right_2 > (side_wall_dist_1 - marginal))))
	{
		set_local_wall(wall_r,0x11);
		set_local_searched(wall_r,0x11);
	}
	else
	{
		if((right_1 < (wall_dist_3 + marginal)) && (right_1 > (wall_dist_3 - marginal)))
		{
			set_local_wall(wall_b,0x21);
			set_local_searched(wall_b,0x21);
			set_local_searched(wall_r,0x11);
		}
		else if((right_1 < (side_wall_dist_2 + marginal)) && (right_1 > (side_wall_dist_2 - marginal)))
		{
			set_local_wall(wall_r,0x20);
			set_local_searched(wall_r,0x20);
			set_local_searched(wall_b,0x21);
			set_local_searched(wall_r,0x11);
		}
		else
		{
			set_local_searched(wall_r,0x20);
			set_local_searched(wall_b,0x21);
			set_local_searched(wall_r,0x11);
		}
		
		if((right_2 < (wall_dist_3 + marginal)) && (right_2 > (wall_dist_3 - marginal)))
		{
			set_local_wall(wall_f,0x21);
			set_local_searched(wall_f,0x21);
			set_local_searched(wall_r,0x11);
		}
		else if((right_2 < (side_wall_dist_2 + marginal)) && (right_2 > (side_wall_dist_2 - marginal)))
		{
			set_local_wall(wall_r,0x22);
			set_local_searched(wall_r,0x22);
			set_local_searched(wall_f,0x21);
			set_local_searched(wall_r,0x11);
		}
		else
		{
			set_local_searched(wall_r,0x22);
			set_local_searched(wall_f,0x21);
			set_local_searched(wall_r,0x11);
		}
	}	
}




//Funktion som s�ger var det finns v�ggar och placerar dessa enligt lokal orientering
//Funktionen f�ruts�tter att measurements[][] p� rad 0 inneh�ller de data som skall utv�rderas
//Funktionen kr�ver �ven mer eller mindre att man befinner sig mitt en den ruta man vill m�ta
//annars kr�vs en st�rre marginal kr�vas vilket skulle kunne leda till att vissa v�ggars 
//intervall �verlappar. Det �r 34 cm mellan side_wall_dist_1 och wall_dist_3 vilka �r de mest kritiska
void look_for_walls()
{
	read_sensors();

	unsigned char sensor_index = direction;
	//FRONT
	unsigned char front = get_measurements(sensor_index, 0);
	look_for_front_wall(front);

	//LEFT
	sensor_index += sensor_index;
	sensor_index = modulo_6(sensor_index);
	unsigned char left_1 = get_measurements(sensor_index, 0);
	sensor_index += sensor_index;
	sensor_index = modulo_6(sensor_index);
	unsigned char left_2 = get_measurements(sensor_index, 0);
	look_for_left_wall(left_1, left_2);	

	//BACK
	sensor_index += sensor_index;
	sensor_index = modulo_6(sensor_index);
	unsigned char back = get_measurements(sensor_index, 0);
	look_for_back_wall(back);

	//RIGHT
	sensor_index += sensor_index;
	sensor_index = modulo_6(sensor_index);
	unsigned char right_1 = get_measurements(sensor_index, 0);
	sensor_index += sensor_index;
	sensor_index = modulo_6(sensor_index);
	unsigned char right_2 = get_measurements(sensor_index, 0);
	look_for_right_wall(right_1, right_2);

	//Skicka inte skiten till Svante h�r
	//V�nta tills roboten har t�nkt lite p� de eventuella luckor 
	//den kan t�ppa till med lite tankekraft			
}


	

//------------------------------------------------------
//		M�tdatahantering
//------------------------------------------------------
//De m�tta sensorv�rdena skall tolkas p� n�got bra s�tt som g�r det m�jligt att
//avg�ra n�r man fortfarande �r "mitt f�r en v�gg" och inte vid ett h�rn
	
//------------------------------------------------------
//		REGLERING
//------------------------------------------------------

//Funktion som ser till att roboten hela tiden befinenr sig p� ett lagom avst�nd fr�n side_wall	
void reglera()
{
	char reglering = 0;
	
	//Ska initiera det accpeterat felet fr�n datorn.
	//Ska allts� inte vara kvar, utan den ska initieras via PC:n!!!
	unsigned char accepterat_fel = 0;

	//Kolla om vi har en v�gg till h�ger, dvs om n�rmaste v�gg ligger till h�ger
	if((get_local_wall(1,1) & 0b0010) >> 1)
	{
		//Ber�knar om vi st�r fel
		char fel = side_wall_dist_1 - side_wall_dist_2;

		//Om felet �verskrider en viss gr�ns, som initeras via datorn, reglerar vi �t h�ger
		if(fel < -accepterat_fel) 
		{
			reglering = fel;
		}
		//Om felet �verskrider en viss gr�ns, som initeras via datorn, reglerar vi �t v�nster
		else if(fel > accepterat_fel)
		{
			reglering = -fel;
		}
		else
		{
			reglering = 0;
		}
	}
	else // Har vi inte v�gg till h�ger s� ska det finnas en v�gg till v�nster
	{
		//Vi �r vid en h�rnkant, ingen reglering
		reglering = 0;
	}

	
	//Ska skicka regleringen till styrenheten
	movement_data[0] = 0; //Skicka bitar som s�ger att det �r regleringen som ska skickas
	movement_data[1] = reglering;
	//Kan vi skicka reglering om det �r ett negativt v�rde, hur l�ser man detta?
}

//Funktion som k�rs endast n�r roboten s�tts ig�ng

void start_up_routine()
{
	//Denna beh�ver endast g�ras en g�ng. Sedan ges sidov�ggen av hur den r�r sig.
	decide_side_wall();
}
	 
//Funktion som s�ger hur roboten skall r�r sig.
void movement_direction()
{
	look_for_walls();
	
	//Kolla om vi har en v�gg till h�ger
	if((get_local_wall(1,1) & 0b0010) >> 1)
	{
		//Kolla om det finns en v�gg fram�r n�san
		if ((get_local_wall(1,1) & 0b0100) >> 2)
		{
			//Sv�ng v�nster
			movement_data[0] = 4;

			//Kom-eneheten h�mtar, l�ter sedan det vara tills kom-eneheten vill ha n�got nytt
			// och d� s�tter man nya data efter �nskem�l.

		}
		//Kolla efter v�gg tv� steg framf�r n�san
		else if ((get_local_wall(1,2) & 0b0100) >> 2)
		{
			if((get_local_wall(0,1) & 0b0100) >> 2)
			{
			//Vi har hittat en �terv�ndsgr�nd. 
			//S�tt De tv� �nnu ej hittade v�ggarna. G�rs eventuellt i storkartan
			
			//Sv�ng V�nster
			movement_data[0] = 4;

			//Den har h�r roterat och bytt riktning. Beh�ver den g� d� ocks� eller ska detta 
			// utf�ras mha g� fram�t? Om inte blir det ganska konstigt med vad som �r fram och vad
			// som �r h�ger och v�nster, eller?
			
			}
			else
			{
			//G� fram�t
			movement_data[0] = 2;

			//inte tillr�ckligt med info f�r att best�mma 
			}
		}
		//Kolla om vi har en �terv�ndsgr�nd fram�t, men eventuellt missar en v�gg.
		else if ((get_local_wall(0,1) & 0b0100) >> 2)
		{
			//Underf�rst� tre v�ggar
			//Sv�ng v�nster			
			movement_data[0] = 4;

		}
		//Om ingen av dessa v�ggar existerar. G� en ruta fram�t
		else
		{
			//G� fram�t
			movement_data[0] = 2;
			
		}
	}
	//Om vi inte har en v�gg till h�ger
	else
	{
		//Kolla om det finns en v�gg fram�r n�san
		if ((get_local_wall(1,1) & 0b0100) >> 2)
		{
			//IOM att vi vet att det inte finns en v�gg till h�ger
			//Har vi i det h�r l�get en �terv�ndsgr�nd d�r.

			//Sv�ng v�nster
			movement_data[0] = 4;

		}
		//Kolla om vi ser in i en �terv�ndsgtr�nd till h�ger
		else if ((get_local_wall(2,1) & 0b0100) >> 2)
		{
			//IOM att vi inte hade en v�gg framf�r oss
			
			//G� fram�t
			movement_data[0] = 2;
			
		}
		//Vi har hittat ett ytterh�rn och skall sv�nga h�ger
		else 
		{
			//Sv�ng h�ger
			movement_data[0] = 5;

		}
	}	
}	
