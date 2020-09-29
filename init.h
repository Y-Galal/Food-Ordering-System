/*
* File name: init.h
* 
* description: a header file for initializing the system.
*
*/


#ifndef INIT_H
#define INIT_H
/* for drivers initialization */
#include "driverlibrary/GPIO.h"
#include "driverlibrary/pll.h"
#include "driverlibrary/uart0.h"
#include "driverlibrary/keypad.h"
#include "driverlibrary/internal_eeprom.h"
#include "driverlibrary/lcd.h"
#include "driverlibrary/std_types.h"


void systemInit();
#endif

