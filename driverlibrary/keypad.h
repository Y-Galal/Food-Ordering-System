 /******************************************************************************
 *
 * Module: Keypad
 *
 * File Name: keypad.h
 *
 * Description: header file for the keypad
 *
 * Author: Youssef Galal
 *
 *******************************************************************************/


#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "GPIO.h"


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/* Keypad configurations for number of rows and columns */
    #define N_col 4

    #define N_row 4

    #define FULL_PORT 0xFF

    #define KEYPAD_PORT_OUT1 (ACCESS_REG(PORTB,GPIODATA))      /*Change these to connect the keypad to another port*/
    #define KEYPAD_PORT_OUT2 (ACCESS_REG(PORTA,GPIODATA))

    #define KEYPAD_PORT_IN PORTA

    #define KEYPAD_PORT_DIR1 (ACCESS_REG(PORTB,GPIODIR))
    #define KEYPAD_PORT_DIR2 (ACCESS_REG(PORTA,GPIODIR))

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void Keypad_init();
uint8 Keypad_getPressedKey(void);


#endif /* KEYPAD_H_ */
