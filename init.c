#include "init.h"

/*
*   Function Name: systemInit
*   description: this function initializes all the drivers that are used in this project
*               
*/
void systemInit()
{
    PLL_setClock(CLK_80);       /*Setting the clock to 80 MHz*/
    Keypad_init();              /*initializing the keypad*/
    EEPROM_init();              /*initializing the internal EEPROM*/
    LCD_init();                 /*initiazliing the LCD*/
    UART_init(80000000,115200); /*initializing UART0*/
    UART_IntEnable();           /*Enabling UART0 Receive Timeout interrupt*/

}
