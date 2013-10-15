#include <adc.h>
#include <reglering.h>

//De olika reglertillstånd som existerar
#define manuell 0
#define start_up 1
#define wall_l 0b1000
#define wall_f 0b0100
#define wall_r 0b0010
#define wall_b 0b0001


//Den tillåtna marginal för vägg detektering. Skall testas
#define marginal 10

//Variabel som ettställs när roboten försäkrat sig om att den står parallellt
//med en vägg. Denna ska nollställas när roboten t ex passaerar ett hörn och 
//inte längre har båda sensorerna mot samma vägg.

unsigned char regulation_mode = manuell;
//Variabel som definierar någon form av global riktning 0-3 (vänster, fram, höger, bak)
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




//Funktion som säger var det finns väggar och placerar dessa enligt lokal orientering
//Funktionen förutsätter att measurements[][] på rad 0 innehåller de data som skall utvärderas
//Funktionen kräver även mer eller mindre att man befinner sig mitt en den ruta man vill mäta
//annars krävs en större marginal krävas vilket skulle kunne leda till att vissa väggars 
//intervall överlappar. Det är 34 cm mellan side_wall_dist_1 och wall_dist_3 vilka är de mest kritiska
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

	//Skicka inte skiten till Svante här
	//Vänta tills roboten har tänkt lite på de eventuella luckor 
	//den kan täppa till med lite tankekraft			
}


	

//------------------------------------------------------
//		Mätdatahantering
//------------------------------------------------------
//De mätta sensorvärdena skall tolkas på något bra sätt som gör det möjligt att
//avgöra när man fortfarande är "mitt för en vägg" och inte vid ett hörn
	
//------------------------------------------------------
//		REGLERING
//------------------------------------------------------

//Funktion som ser till att roboten hela tiden befinenr sig på ett lagom avstånd från side_wall	
void reglera()
{
	char reglering = 0;
	
	//Ska initiera det accpeterat felet från datorn.
	//Ska alltså inte vara kvar, utan den ska initieras via PC:n!!!
	unsigned char accepterat_fel = 0;

	//Kolla om vi har en vägg till höger, dvs om närmaste vägg ligger till höger
	if((get_local_wall(1,1) & 0b0010) >> 1)
	{
		//Beräknar om vi står fel
		char fel = side_wall_dist_1 - side_wall_dist_2;

		//Om felet överskrider en viss gräns, som initeras via datorn, reglerar vi åt höger
		if(fel < -accepterat_fel) 
		{
			reglering = fel;
		}
		//Om felet överskrider en viss gräns, som initeras via datorn, reglerar vi åt vänster
		else if(fel > accepterat_fel)
		{
			reglering = -fel;
		}
		else
		{
			reglering = 0;
		}
	}
	else // Har vi inte vägg till höger så ska det finnas en vägg till vänster
	{
		//Vi är vid en hörnkant, ingen reglering
		reglering = 0;
	}

	
	//Ska skicka regleringen till styrenheten
	movement_data[0] = 0; //Skicka bitar som säger att det är regleringen som ska skickas
	movement_data[1] = reglering;
	//Kan vi skicka reglering om det är ett negativt värde, hur löser man detta?
}

//Funktion som körs endast när roboten sätts igång

void start_up_routine()
{
	//Denna behöver endast göras en gång. Sedan ges sidoväggen av hur den rör sig.
	decide_side_wall();
}
	 
//Funktion som säger hur roboten skall rör sig.
void movement_direction()
{
	look_for_walls();
	
	//Kolla om vi har en vägg till höger
	if((get_local_wall(1,1) & 0b0010) >> 1)
	{
		//Kolla om det finns en vägg framör näsan
		if ((get_local_wall(1,1) & 0b0100) >> 2)
		{
			//Sväng vänster
			movement_data[0] = 4;

			//Kom-eneheten hämtar, låter sedan det vara tills kom-eneheten vill ha något nytt
			// och då sätter man nya data efter önskemål.

		}
		//Kolla efter vägg två steg framför näsan
		else if ((get_local_wall(1,2) & 0b0100) >> 2)
		{
			if((get_local_wall(0,1) & 0b0100) >> 2)
			{
			//Vi har hittat en återvändsgränd. 
			//Sätt De två ännu ej hittade väggarna. Görs eventuellt i storkartan
			
			//Sväng Vänster
			movement_data[0] = 4;

			//Den har här roterat och bytt riktning. Behöver den gå då också eller ska detta 
			// utföras mha gå framåt? Om inte blir det ganska konstigt med vad som är fram och vad
			// som är höger och vänster, eller?
			
			}
			else
			{
			//Gå framåt
			movement_data[0] = 2;

			//inte tillräckligt med info för att bestämma 
			}
		}
		//Kolla om vi har en återvändsgränd framåt, men eventuellt missar en vägg.
		else if ((get_local_wall(0,1) & 0b0100) >> 2)
		{
			//Underförstå tre väggar
			//Sväng vänster			
			movement_data[0] = 4;

		}
		//Om ingen av dessa väggar existerar. Gå en ruta framåt
		else
		{
			//Gå framåt
			movement_data[0] = 2;
			
		}
	}
	//Om vi inte har en vägg till höger
	else
	{
		//Kolla om det finns en vägg framör näsan
		if ((get_local_wall(1,1) & 0b0100) >> 2)
		{
			//IOM att vi vet att det inte finns en vägg till höger
			//Har vi i det här läget en återvändsgränd där.

			//Sväng vänster
			movement_data[0] = 4;

		}
		//Kolla om vi ser in i en återvändsgtränd till höger
		else if ((get_local_wall(2,1) & 0b0100) >> 2)
		{
			//IOM att vi inte hade en vägg framför oss
			
			//Gå framåt
			movement_data[0] = 2;
			
		}
		//Vi har hittat ett ytterhörn och skall svänga höger
		else 
		{
			//Sväng höger
			movement_data[0] = 5;

		}
	}	
}	
