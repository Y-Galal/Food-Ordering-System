 /******************************************************************************
 *
 * Module: Keypad
 *
 * File Name: keypad.c
 *
 * Description: source file for the keypad
 *
 * Author: Youssef Galal
 *
 *******************************************************************************/



#include "keypad.h"

void Keypad_init()
{
    uint8 i;
    for(i=4;i<8;i++)
    {
	PIN_CONFIG port_config = {PORTB,i,INPUT,PULL_UP,TWO};
	GPIO_configureDigitalPin(&port_config);
    }
    for(i=4;i<8;i++)
    {
        PIN_CONFIG port_config = {PORTA,i,INPUT,PULL_UP,TWO};
        GPIO_configureDigitalPin(&port_config);
    }
}





/*********************************************************************************/

#if (N_col == 3)

static uint8 KeyPad_4x3_adjustSwitchNumber(uint8 button_number);
#elif ( N_col == 4 )

static uint8 KeyPad_4x4_adjustSwitchNumber(uint8 button_number);
#endif

uint8 Keypad_getPressedKey(void)
{
	volatile uint8 col,row;

	while(1)
	{
		for(col=0; col<N_col;col++)
		{
            KEYPAD_PORT_DIR1 = (0b00010000 << col)&(0xf0); /* we open one column each time and close every column and the rows too*/
            KEYPAD_PORT_DIR2 = (0b00000001 << col)&(0xF0);
            KEYPAD_PORT_OUT1 = (~(0b00010000<<col)); /* we clear the selected column to make it LOW and enable pull-up resistors for rows pins */
            KEYPAD_PORT_OUT2 = (~(0b00000001<<col) & (0xF0));

			for(row=0; row<N_row; row++)
			{
				if(	BIT_IS_CLEAR(	KEYPAD_PORT_IN,GPIODATA,row+4)	)
				{
					#if (N_col == 3)
					return KeyPad_4x3_adjustSwitchNumber((row*N_col)+1+col);
					#elif (N_col == 4 )
					return KeyPad_4x4_adjustSwitchNumber((row*N_col)+1+col);
					#endif
				}
			}
		}
	}
}

#if ( N_col == 3 )
static uint8 KeyPad_4x3_adjustSwitchNumber(uint8 button_number)
{
	switch (button_number)
	{
	case 10 : return '*';
				break;
	case 11 : return 0;
				break;
	case 12 : return '#';
				break;
	default : return button_number;
	}
}
#elif ( N_col == 4 )
static uint8 KeyPad_4x4_adjustSwitchNumber(uint8 button_number)
{
	switch(button_number)
	{
	case 1 : return 1 ;

	case 2 : return 2;

	case 3 : return 3;

	case 4 : return 'A' ;

	case 5 : return 4 ;

	case 6 : return 5 ;

	case 7 : return 6;

	case 8 : return 'B';

	case 9 : return 7;

	case 10 : return 8;

	case 11 : return 9;

	case 12 : return 'C';

	case 13 : return '*';

	case 14 : return 0;

	case 15 : return '#';

	case 16 : return 'D' ;

	default : return button_number;
	}
}

#endif
/*
 * 		4x4 KeyPad
 * 		1 2 3 A
 * 		4 5 6 B
 * 		1 2 3 C
 * 		* 0 # D
 *
 * 		4x3 KeyPad
 * 		1 2 3
 * 		4 5 6
 * 		7 8 9
 * 		* 0 #
 *
 */







