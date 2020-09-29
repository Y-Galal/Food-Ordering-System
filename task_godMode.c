#include "imp_headers.h"

/*This array has the strings of food names*/
const uint8 *food[9] = {
                    "Akeel Chicken",
                    "Akeel Burger",
                    "Mighty Fillet",
                    "French Fries",
                    "Cheesy Fries",
                    "Coleslaw",
                    "Pepsi",
                    "Cocacola",
                    "7UP",
                    };

#define SAVED_FOOD_FLAG_ADD 0x10        /*an address of the flag that indicates that the system is starting for the first time or not*/
#define SAVED_FOOD_FLAG_DATA 0x10       /*Data that will be saved in a known address to indicate that the system started at least one time. */

#define MEMORY_BLOCK1   1               /*Memory block that has the password*/
#define PASSWORD_ADDRESS 0              /*The base address of the first character in the password*/

/*Static functions prototypes*/

static void ASCIIToValue(uint8 *asciiarr ,uint32 *converted);
static void saveFood();
static uint8 getPassword();
static void changePassword();
static uint8 comparePasswords(uint8 *password1,uint8 *password2);
static void savePassword(uint8 *passwordToSave);
static void setPassword();
static void showFood();
static void changeQuantityInMenu();

/*
    Function Name: UART_callBack
    Description: this function will be called when the user sends data from pc to the system to enter god mode.
*/
void UART_callBack()
{
    if(BIT_IS_SET(UART0,UART_RIS,RTIS))     /*if the interrupt sent is receive timeout interrupt...*/
    {
    SET_BIT(UART0,UART_ICR,RTIC);           /*Clearing the interrupt as we will handle it now*/
    BaseType_t HigherPriorityTaskWoken=pdFALSE;
    uint8 data = UART_receiveByte();        /*receiving the data sent from pc*/
    xQueueSendFromISR(Queue_godModeRequest,&data,&HigherPriorityTaskWoken); /*sending the data to the queue to process it in the task*/
    portYIELD_FROM_ISR(HigherPriorityTaskWoken);        /*Allowing context switching*/
    }
}

/*
    Function Name: vTaskGodMode
    Description: This is the main code of the god mode task
*/
void vTaskGodMode(void *pvParameters)
{
    uint32 checkForSavedFood;   /*Variable to read the flag value to determine whether the system is starting for the first time or not*/
    uint8 godModeValue; /*Variable to get the data sent from the owner to perform a request*/
    EEPROM_readData(MEMORY_BLOCK, SAVED_FOOD_FLAG_ADD,&checkForSavedFood);  /*reading the flag from the EEPROM*/

    if(checkForSavedFood == SAVED_FOOD_FLAG_DATA)       /*if this condition is true, this means that the system is starting for the frist time
                                                            So we neet to set a password and save the food quantity for the first time*/
    {
        EEPROM_writeData(MEMORY_BLOCK,  SAVED_FOOD_FLAG_ADD, SAVED_FOOD_FLAG_DATA); /*Indicating that food is saved*/
        setPassword();      /*Setting the password*/
        saveFood();         /*Saving the food for the first time*/
    }

    /*Sending some messages to the owner using his serial terminal (PuTTy)*/
    UART_sendString("This is God Mode for the owner only...");  
    UART_sendString("Press '1' to change password");
    UART_sendString("Press '2' to change the food availability");
    UART_sendString("Press '3' to show the current quantity of food");
    UART_sendString("Press '4' to change 1 specific food quantity in the menu");
    while(1)
    {
        xQueueReceive(Queue_godModeRequest,&godModeValue,portMAX_DELAY);    /*This task will remain blocked until there is a request in the queue*/
        UART_sendString("God Mode Entered!");   
        switch (godModeValue)   /*We can take the values 1,2,3,4 from the owner to do some requests*/
        {
        case '1':   /*Change Password*/
            changePassword();   
            break;
        case '2':   /*Change Food Availablility*/
            UART_sendString("Change Food Items... Enter Password");
            if(getPassword())   /*if the password the user enters is correct we will serve his request, otherwise the request will be refuesed*/
            {
                saveFood();
            }
            else
            {
                UART_sendString("WRONG PASSWORD.. YOUR REQUEST HAS BEEN REFUSED");
            }

            break;
        case '3':       /*To show the owner the food availablility*/
            UART_sendString("Enter Password");

            if(getPassword())
            {
                UART_sendString("Showing the food available...");
                showFood();
            }
            else
            {
                UART_sendString("WRONG PASSWORD.. YOUR REQUEST HAS BEEN REFUSED");
            }
            break;
        case '4':   /*if the user wants to change only 1 quatity of food there*/
            UART_sendString("Enter Password");
            if(getPassword())
            {
                changeQuantityInMenu();
            }
            else
            {
                UART_sendString("WRONG PASSWORD.. YOUR REQUEST HAS BEEN REFUSED");
            }
            break;
        default:

            break;
        }
    }
}

/*
    Function Name: ASCIIToValue
    Description:  This function takes a string of numbers in ascii format and converts it to an actual value
                    i.e. converts 0x30 (0 in ascii) to the value 0
*/
static void ASCIIToValue(uint8 *asciiarr ,uint32 *converted)
{
    uint8 i=0;
    *converted =0;
    while(asciiarr[i] != '\0')
    {
        *converted = *converted * 10;
        *converted += asciiarr[i] - ASCII_VALUE_BASE;

        i++;
    }
}

/*
    Function Name: saveFood
    Description: this function takes the food quantity from the owner and saves them in the EEPROM
*/
static void saveFood()
{
    uint8 i, quantity[4];
    uint32 convertedValue=0;
    UART_IntDisable();
    UART_sendString("Enter the number of the available items");
    for(i=0; i<9; i++)      
    {
        UART_sendString(food[i]);       /*Sending the food name to the user*/

        UART_receiveString(quantity);   /*Receiving the food quantity from the user*/
        
        ASCIIToValue(quantity,&convertedValue); /*Converting it from ASCII to an actual value*/
        EEPROM_writeData(MEMORY_BLOCK, i, convertedValue) ;    /*Saving this value to the EEPROM*/
    }
    UART_sendString("Food Saved!");
    UART_IntEnable();
}

/*
    Function Name: getPassowrd
    Description: This function retreives the password saved in the EEPROM and takes the password form the user
                then compares them then returns 1 if the passwords are matched and 0 if the passwords didn't match
*/
static uint8 getPassword()
{
    uint8 password[6]; uint8 savedPassword[6],i=0; uint32 dataFromEEPROM;
    for(i=0;i<6;i++)
    {
        EEPROM_readData(MEMORY_BLOCK1,PASSWORD_ADDRESS+i, &dataFromEEPROM );    /*getting the password saved in the EEPROM*/
        savedPassword[i]= (uint8)dataFromEEPROM;
    }
    UART_IntDisable();  
    UART_receiveString(password);       /*Getting the password from the user*/
    UART_IntEnable();
    return comparePasswords(password,savedPassword);    /*comparing them and returning either 1 or 0*/
}


/*
    Function Name: changePassword
    Description: this function handles the change password request
                the owner enters the old password if the password is correct then he can change the password
*/
static void changePassword()
{
    UART_sendString("Change Password Selected.. Enter old password");
   
    if( getPassword() ) 
    {
        setPassword();
    }
    else
    {
        UART_sendString("WRONG PASSWORD.. YOUR REQUEST HAS BEEN REFUSED");
    }
    
}

/*
    Function Name: comparePasswords
    Description: this function compares two passwords and returns 1 if they are matched and 0 if the didn't match
*/
static uint8 comparePasswords(uint8 *password1,uint8 *password2)
{
    uint8 i=0;
    for(i=0;i<6;i++)
    {
        if(password1[i] != password2[i])
        {
            return 0;
        }
    }
    return 1;
}

/*
    Function Name: savePassword
    Description: this function saves the password in the EEPROM
*/
static void savePassword(uint8 *passwordToSave)
{
    uint8 i=0;
    for(i=0;i<6;i++)
    {
        EEPROM_writeData(MEMORY_BLOCK1,PASSWORD_ADDRESS+i, passwordToSave[i]);
    }
}

/*
    Function Name: setPassword
    Description: This function takes the password from the owner and saves the password
*/
static void setPassword()
{
    uint8 password[6],matched=0; uint8 passwordVerify[6];
    while(!matched)
    {
        UART_sendString("Hello, Owner.. Please Set Password");  
        UART_IntDisable();
        UART_receiveString(password);           /*getting the password for the first time*/
        UART_sendString("Enter Password Again..");      
        UART_receiveString(passwordVerify);     /*getting the password again to confirm it*/
        UART_IntEnable();
        matched = comparePasswords(password,passwordVerify) ;   /*if the passwords are matched then we will save it*/
    }
    savePassword(password); /*saving the password*/
}

/*
    Function Name: showFood
    Description:  This function reads the food quantity form the EEPROM and sends it to the owner
*/
static void showFood()
{
    uint8 i; uint32 foodQuantity;
    for(i=0; i<9; i++)
    {
        UART_sendString(food[i]);                           /*Showing the food name*/
        EEPROM_readData(MEMORY_BLOCK, i, &foodQuantity) ;   /*reading the food quantity from the EEPROM*/
        UART_sendNumber((uint16)foodQuantity);              /*Showing the food quantity*/
    }
}

/*
    Function Name: changeQuantityInMenu
    Description: This function enables the owner to change only 1 quantity of food in the menu
*/
static void changeQuantityInMenu()
{
    uint8 dataInASCII[4];
    uint32 convertedValue=0,foodType=0;
    UART_IntDisable();
    UART_sendString("Enter The number 0 to 8 of the food you want to change its quantity");
    UART_receiveString(dataInASCII);        /*getting the food number from 0 to 8*/
    ASCIIToValue(dataInASCII,&foodType);    
    if(foodType <=8)
    {
        UART_sendString(food[foodType]);        /*Showing the food name*/
        UART_sendString("Enter food quantity"); 
        UART_receiveString(dataInASCII);        /*Entering new quantity*/
        ASCIIToValue(dataInASCII,&convertedValue);  /*Converting it*/
        EEPROM_writeData(MEMORY_BLOCK, (uint8)foodType, convertedValue) ; /*saving the new quantity in the EEPROM*/
    }
    UART_sendString("Food Saved!");
    UART_IntEnable();
}
