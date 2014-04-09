/*
 * main.c
 *
 *  Created on: Apr 9, 2014
 *      Author: mateuszmmi
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "keyb.h"

//define pins used by buttons-----------------------------
#define KEY_DDR			DDRD
#define KEY_PORT	   	PORTD
#define KEY_PIN			PIND
//other definitions in external keyb.h file
//--------------------------------------------------------

//define output's-----------------------------------------
#define OUT_DDR    		DDRB
#define OUT_PORT   		PORTB

#define back1			PB1	//back light 1
#define back2			PB2	//back light 2
#define longlight		PB0	//long light
#define shortlight		PB3	//short light
#define breaks			PC3 //Break lights
#define buzzer			PD7 //horn of abbys :)
#define left			PB6 //left indicator
#define right			PB7 // right indicator
#define pwm_left_back	OCR1A //left backlight with pwm
#define pwm_right_back	OCR1B
#define pwm_short_front	 OCR2
#define checkbutton	 	PB5
//--------------------------------------------------------

//Define voltage meter LED--------------------------------
#define LEDV_DDR		DDRC
#define LEDV_PORT		PORTC
#define blue			PC2 //blue led
#define green			PC1 //green led
#define red				PC0 //red led

#define red_OFF			LEDV_PORT |= (1<<red)
#define red_ON			LEDV_PORT &=~ (1<<red)
#define green_OFF		LEDV_PORT |= (1<<green)
#define green_ON		LEDV_PORT &=~ (1<<green)
#define blue_OFF		LEDV_PORT |= (1<<blue)
#define blue_ON			LEDV_PORT &=~ (1<<blue)
//---------------------------------------------------------
#define ADCIN PC5   //ADCIN definition (input ADC)

//Program variables
unsigned char  i, j, k=0;
char duty[]={0, 0, 1, 2, 3, 5, 9, 14, 20, 27, 36, 41, 46, 50, 53, 55, 55, 53, 50, 46, 41, 36, 27, 20, 14, 9, 5, 3, 2, 1, 0, 0};
char dutyshort[]={0, 3, 20, 90, 255, 90, 20, 3};
volatile unsigned char a=0, b, e=0, f=0 ;
volatile unsigned int g=0, l;
static char _key;
static unsigned int _key_time;


/*--------------------------------------------------------------------------
  Prototypes
--------------------------------------------------------------------------*/
unsigned char get_key_press( unsigned char key_mask );
void init_timers(void);
void init_io(void);

/*--------------------------------------------------------------------------
						 Sets and starts ADC
--------------------------------------------------------------------------*/

void adc_init()
{
// AREF = 2,56V
 ADMUX = (1<<REFS1) | (1<<REFS0)
		|(1<<MUX2) | (1<<MUX0);// wybór kana?u ADC5 na pinie PC5

ADCSRA = (1<<ADEN) //w??czenie ADC
		   |(1<<ADFR) //w??czenie trybu Free run
		   |(1<<ADIE) //uruchomienie zg?aszania przerwa?
		   |(1<<ADSC) //rozpocz?cie konwersji
		   |(1<<ADPS0)   //ADPS2:0: ustawienie preskalera na 128
		   |(1<<ADPS1)
		   |(1<<ADPS2);
}






/*--------------------------------------------------------------------------
					 Sets and starts a system timer
--------------------------------------------------------------------------*/
void init_timers(void)
{
	cli();            // read and clear atomic !
	//Timer0 for buttons
	TCCR0 |= 1<<CS02 | 1<<CS00;  //Divide by 1024
	TIMSK |= 1<<TOIE0;     //enable timer overflow interrupt
	sei();            // enable interrupts

	//			Timer counter 1 initalization
	TCCR1A |= (1<<COM1A1) |(1<<COM1B1) |(1<<COM1A0) |(1<<COM1B0) | (1<<WGM10); //PWM, Phase Correct, 8-bit initalize
	TCCR1B |= (1<<CS11)| (1<<CS10) ;														// 1/1024 preskalling

	//			TimerCounter2 initalization


	TCCR2 |= (1<<WGM20) | (1<<COM21) | (1<<COM20) |(1<<CS22)|(1<<CS20);

}

/*--------------------------------------------------------------------------
					 Initialize input and output registers
--------------------------------------------------------------------------*/
void init_io(void)
{
	//Setup Buttons
	KEY_DDR &= ~(KEY0 | KEY1 | KEY2 | KEY3 | KEY4 | KEY5 | KEY6 );  //Set pins as input
	OUT_DDR &= ~ (1<<checkbutton);
	KEY_PORT |= KEY0 | KEY1 | KEY2 | KEY3 | KEY4 | KEY5 | KEY6;    //enable pull-up resistors
	OUT_PORT |= (1<<checkbutton);

	LEDV_DDR &=~ (1<<ADCIN);            //Ustawienie Wej?cia ADC

	//------------------Setup LEDs and buzzer------------------------------------------

	OUT_DDR |= (1<<shortlight) | (1<<longlight) | (1<<back1) | (1<<back2) | (1<<left) |(1<<right);		//Set pins as output
	LEDV_DDR |= (1<<breaks) |(1<<green)|(1<<blue)|(1<<red);
	DDRD |= (1<<buzzer);


	OUT_PORT |= (1<<shortlight) | (1<<longlight) | (1<<back1) | (1<<back2) | (1<<left) |(1<<right);	//initial value off
	LEDV_PORT |= (1<<breaks) |(1<<green)|(1<<blue)|(1<<red);
	KEY_PORT |= (1<<buzzer);
}





// delay loop with key checking
void delay(unsigned char c)
{
	for (i=0; i<=c; i++)
		_delay_ms(5);
}


int main(void)
{
	adc_init();		//initalize ADC
	init_timers();    //start the timer
	init_io();        //setup the buttons

	sei();

	while(1) //eternal loop
	{
		START:


		switch(a)
		{

			// ================ LED's off========================
			case 0:
			{

				pwm_left_back=0;
				pwm_right_back=0;
			}	break;

			//===== Smooth left-right on back==================
			case 1:
			{
				for (j=0; j <= 15; j++)
				{
					pwm_left_back= duty[j];
					pwm_right_back=	duty[16-j];
					delay(7);
					if (a!=1)
						goto START;
				}

				if (a!=1)
					goto START;

				for (j=0; j <= 15; j++)
				{
					pwm_right_back= duty[j];
					pwm_left_back=	duty[16-j];
					delay(7);
					if (a!=1)
						goto START;
				}


			} break;

			//========= roof-top sign, POLICEMAN's COCK==============
			case 3:
			{
				for (j=0; j <= 15; j++)
				{
					pwm_left_back= duty[j];
					_delay_ms(6);
				}

				for (j=0; j <= 15; j++)
				{
					pwm_left_back=	duty[16-j];
					_delay_ms(6);
				}

				if (a!=3)
					goto START;

				for (j=0; j <= 15; j++)
				{
					pwm_left_back= duty[j];
					_delay_ms(6);
				}

				for (j=0; j <= 15; j++)
				{
					pwm_left_back=	duty[16-j];
					_delay_ms(6);
				}

				if (a!=3)
					goto START;
				for (j=0; j <= 15; j++)
				{
					pwm_right_back=	duty[j];
					_delay_ms(6);
				}

				for (j=0; j <= 15; j++)
				{
					pwm_right_back= duty[16-j];
					_delay_ms(6);
				}

				for (j=0; j <= 15; j++)
				{
					pwm_right_back=	duty[j];
					_delay_ms(6);
				}

				if (a!=3)
					goto START;

				for (j=0; j <= 15; j++)
				{
					pwm_right_back= duty[16-j];
					_delay_ms(6);
				}

			} break;

			//============smooth on-off both LED===============
			case 2:
			{
				for (j=0; j <= 15; j++)
				{
					pwm_left_back= duty[j];
					pwm_right_back= duty[j];
					delay(9);
					if (a!=2)
						goto START;
				}

				for (j=0; j <= 15; j++)
				{
					pwm_right_back= duty[16-j];
					pwm_left_back=	duty[16-j];
					delay(1);
					if (a!=2)
						goto START;
				}

			} break;

			// ===================LED's ON===========================
			case 4:
			{
				pwm_left_back=80;
				pwm_right_back=80;

			} break;

			// ==================HORN_BLOWING=========================
			case 5:
			{
				OUT_PORT |= (1<<left);
				OUT_PORT |= (1<<right);
				KEY_PORT |= (1<<buzzer);
				for (i=0; i<=250; i++)
				{
					KEY_PORT ^= (1<<buzzer);
					_delay_loop_2(150);
					if(pwm_right_back==0)
					{
						pwm_right_back=80;
						pwm_left_back=80;
					}
					 else
					{
						pwm_right_back=0;
						pwm_left_back=0;
					}
				}
				_delay_ms(50);

				if((KEY_PIN & KEY_HORN))
				{
					a=b;
					KEY_PORT |= (1<<buzzer);
				}


				if (e==1)
				e=0;

			} break;


			//==============LONG_LIGHT_ALLERT=========================
			case 6:
			{
				OUT_PORT |= (1<<left) ;
				OUT_PORT |= (1<<right) ;
				OUT_PORT |= (1<<buzzer);
				for (j=0; j<=3; j++)
				{
					OUT_PORT ^= (1<<longlight);
					if(OCR1B==0)
					{
						pwm_right_back=80;
						pwm_left_back=80;
					}
					else
					{
						pwm_right_back=0;
						pwm_left_back=0;
					}
					delay(36);
				}

				e=0;
				a=f;
			}break;

			// ======================INDICATOR LEFT=======================
			case 7:
			{
				if (g==0)
				{
					pwm_right_back=0;
					pwm_left_back=0;
				}
				else
				{
					pwm_right_back=80;
					pwm_left_back=80;
				}

				if (l<=400)
				{
					 OUT_PORT ^= (1<<left) ;
					 _delay_ms(1);
					 l++;
				}
				else
				{
					KEY_PORT &=~ (1<<buzzer);
					OUT_PORT |= (1<<left) ;
					l=0;
					_delay_ms(400);
					KEY_PORT |= (1<<buzzer);
				}

			} break;


			// ======================INDICATOR RIGHT======================
			case 8:
			{
				if (g==0)
				{
					pwm_right_back=0;
					pwm_left_back=0;
				}
				else
				{
					pwm_right_back=80;
					pwm_left_back=80;
				}
				if (l<=400)
				{
					OUT_PORT ^= (1<<right) ;
					_delay_ms(1);
					l++;
				}
				else
				{
					KEY_PORT &=~ (1<<buzzer);
					OUT_PORT |= (1<<right) ;
					l=0;
					_delay_ms(400);
					KEY_PORT |= (1<<buzzer);
				}
			} break;

			//====================HAZARD LIGHTS==========================
			case 9:
			{
				if (g==0)
				{
					pwm_right_back=0;
					pwm_left_back=0;
				}
				else
				{
					pwm_right_back=80;
					pwm_left_back=80;
				}

				if (l<=400)
				{
					OUT_PORT ^= (1<<right) ;
					OUT_PORT ^= (1<<left) ;
					_delay_ms(1);
					l++;
				}
				else
				{
					KEY_PORT &=~ (1<<buzzer);
					OUT_PORT |= (1<<right) ;
					OUT_PORT |= (1<<left) ;
					l=0;
					_delay_ms(400);
					KEY_PORT |= (1<<buzzer);
				}
			} break;


		}

	}

}





//===================-----------------INTERRUPT------------------------------------------




ISR(TIMER0_OVF_vect)           // interrupt every 10ms
{
	KeybProc();	// Check if button pressed
	cli();
	//---------------------------------------------------

	//------------BREAKS_KEY_PRESSED---------------------
	if(!(KEY_PIN & KEY_BREAK))
		LEDV_PORT ^= (1<<breaks);
	else
		LEDV_PORT |= (1<<breaks);


	//--------------------------------------------------

	//------------SHORT_KEY_PRESSED---------------------


	if( IsKey( KEY_SHORT ))
	{
		_key = 1;
		_key_time = KeysTime();
	}

	if( IsKey( KEY_SHORT ) && KeysTime( ) >= 100 )
	{
		ClrKeyb( KBD_LOCK );	// autorepetition blocking
		if (pwm_short_front<=6)
		{
			pwm_short_front=255;
			k=4;
		}
		else
		{
			pwm_short_front=0;
			k=0;
		}
	}
	else
		if( ((KEY_PIN & KEY_SHORT)) && ( _key_time < 100) )
		{
			if (_key == 1)
				{
					_key=0;
					ClrKeyb( KBD_LOCK );	// autorepetition blocking

					if (k<7)
						k=k+1;
					else
						k=0;

					pwm_short_front=dutyshort[k];
				}
		}

	//======================================================================================

	//=================KEY_LONG_PRESSED=====================================================


	if( IsKey( KEY_LONG ))

	{
		_key = 2;
		_key_time = KeysTime();
	}


	if( IsKey( KEY_LONG ) && KeysTime( ) >= 100 )
	{
		ClrKeyb( KBD_LOCK );	// autorepetition blocking
		OUT_PORT ^= (1<<longlight);
	}
	else
		if( ((KEY_PIN & KEY_LONG)) && ( _key_time < 100) )
		{
			if (_key == 2)
			{
				_key=0;
				if ((e==0) & (OUT_PORT & 0b00000001) & (a!=6))
				{
					f=a;
					a=6;
					OCR1B=0;
					OCR1A=0;
					e=1;
				}
				else
				{
					OUT_PORT |= (1<<longlight);
				}
			}
		}


	//=================================================================================

	//------------HORN_KEY_PRESSED---------------------


	if(!(KEY_PIN & KEY_HORN))
	{
		if ((a!=5) & (a!=6))
		{
			b=a;
			a=5;
		}
	}
	else
	{
		if (a==5)
		a=b;
	}

	//----------------------------------------------------------------

	//----------------------------------------------------------------
	switch( GetKeys() )
	{
	//----------BACK_KEY_PRESSED--------------------------------------
	case KEY_BACK:

		ClrKeyb( KBD_LOCK );	// autorepetition blocking
		if ((a==8) | (a==7) | (a==9))
		{
			a=g-1;
			OUT_PORT |= (1<<left) ;
			OUT_PORT |= (1<<right) ;
			KEY_PORT |= (1<<buzzer);
			l=0;
		}

		if (a<4)
			a++;
		else
			a=0;

		break;

		//-------------------------------------------------------------------

		//----------LEFT_KEY_PRESSED-----------------------------------------
	case KEY_LEFT:
		ClrKeyb( KBD_LOCK );	// autorepetition blocking

		if ((a!=8) & (a!=7) & (a!=9))
		{
			g=a;
			a=7;
		}
		else
		{
			a=g;
			OUT_PORT |= (1<<left) ;
			OUT_PORT |= (1<<right) ;
			KEY_PORT |= (1<<buzzer);
			l=0;
		}

		break;

		//-------------------------------------------------------------------

		//----------LEFT_&_RIGHT_KEY_PRESSED----------------------------------

	case  KEY_LEFT | KEY_RIGHT:
	{
		if ((a!=8) & (a!=7) & (a!=9))
		{
			g=a;
			a=9;
		}
	}
	break;

	//-----------------------------------------------------------------

	//----------RIGHT_KEY_PRESSED--------------------------------------

	case KEY_RIGHT:
		ClrKeyb( KBD_LOCK );	// autorepetition blocking
		if ((a!=8) & (a!=7) & (a!=9))
		{
			g=a;
			a=8;
		}
		else
		{
			a=g;
			OUT_PORT |= (1<<left) ;
			OUT_PORT |= (1<<right) ;
			KEY_PORT |= (1<<buzzer);
			l=0;
		}

		break;
		//-----------------------------------------------------------------

	default:


		break;
	}

	if( IsKey( ANYKEY ))
	{

	}


	TCNT0 = 171;
	sei();
}

//============================ADC======================================

///*

ISR(ADC_vect)//obs?uga przerwania po zako?czeniu konwersji ADC
{
	if(!(PINB & (1<<checkbutton)))
	{
		if ((ADC >=  999) )
		{
			green_OFF;
			red_OFF;
			blue_ON;
		}
		else
		{
			if (ADC >= 500 )
			{
				green_ON;
				red_OFF;
				blue_OFF;
			}

			else
			{
				if (ADC >= 450)
				{
					green_ON;
					red_ON;
					blue_OFF;
				}
				else
				if (ADC >= 400)
				{
					green_OFF;
					red_ON;
					blue_OFF;
				}
			}
		}
	}
	else
	{
		green_OFF;//OFF
		red_OFF;	//OFF
		blue_OFF;	//OFF

	}
}

//	*/
