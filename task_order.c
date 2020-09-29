#include "imp_headers.h"

#define MAIN 1
#define SIDE 2
#define DRINK 3
/*getting the food names to display them in the LCD*/
extern const uint8 *food[9];
uint8 foodOffset[9];    /*An array to check the food availablility*/

/*
    Function Name: food Missing Check
    Description: This function checks if any food is missing, if there is anything missing we will store this in the foodOffset array
*/
static void foodMissingCheck(uint8 *foodOffset)
{
    uint8 i;    uint32 savedQuantity;
    for(i=0;i<9;i++)
    {
        EEPROM_readData(MEMORY_BLOCK, FOOD_QUANTITY_ADD+i, &savedQuantity);
        if(savedQuantity == 0 )
        {
            foodOffset[i]=1;
        }
        else
        {
            foodOffset[i]=0;
        }
    }
}

/*
    Function Name: display Food
    Description: This function displays the food available in the LCD 
*/
static void displayFood(uint8 Type)
{
    LCD_clearScreen();
    switch (Type)
    {
        case MAIN:          /*Displaying the main dish*/
            if(foodOffset[0] != 1)
            {
                LCD_displayStringRowColumn(0,0,"1-Akeel Chicken");
            }
            if(foodOffset[1] != 1 )
            {
                LCD_displayStringRowColumn(1,0,"2-Akeel Burger");
            }
            if(foodOffset[2] !=1)
            {
                LCD_displayStringRowColumn(2,0,"3-Mighty Fillet");
            }
        break;
        case SIDE: /*Displaying the side dish*/
            if(foodOffset[3] != 1)
            {
                LCD_displayStringRowColumn(0,0,"1-French Fries");
            }
            if(foodOffset[4] !=1 )
            {
                LCD_displayStringRowColumn(1,0,"2-Cheesy Fries");
            }
            if(foodOffset[5] !=1)
            {
                LCD_displayStringRowColumn(2,0,"3-Coleslaw");
            }
        break;
        case DRINK: /*displaying the drinks*/
            if(foodOffset[6] != 1)
            {
                LCD_displayStringRowColumn(0,0,"1-Pepsi");
            }
            if(foodOffset[7] !=1 )
            {
                LCD_displayStringRowColumn(1,0,"2-Cocacola");
            }
            if(foodOffset[8] !=1)
            {
                LCD_displayStringRowColumn(2,0,"3-7UP");
            }
        break;
        default:
        break;
    }
    LCD_goToRowColumn(3,0);
}

/*
    Function Name: getChoice
    Description: This function takes the choice from the user to order the food 
*/
static uint8 getChoice(sint8 foodType)
{
    uint8 key = 0;
    /*we will exit this loop when the user enters 1,2 or 3. Also the food must be available so we are checking in the loop too.*/ 
    while(     (key != 1 || foodOffset[foodType+1] == 1)       
            && (key != 2 || foodOffset[foodType+2] == 1)
            && (key != 3 || foodOffset[foodType+3] == 1)
            )
    {
        key = Keypad_getPressedKey();   /*Getting the key pressed from the keypad*/
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
    LCD_displayCharacter( key+ASCII_VALUE_BASE );   /*Displaying the choice*/
    LCD_displayString(" Selected!");
    return key;         /*returning the key pressed value*/
}

/*
    Function Name: displayOrder
    Description: this function displays the order to the user after he finishes his order to confirm or cancel his request
*/
static void displayOrder(uint8 *values)
{
    LCD_clearScreen();
    LCD_displayString("Your order is");
    LCD_displayStringRowColumn(1,0,food[ values[0] + MAIN_DISH_BASE]);
    LCD_displayStringRowColumn(2,0,food[ values[1] + SIDE_DISH_BASE ]);
    LCD_displayStringRowColumn(3,0,food[ values[2] + DRINK_BASE  ]);
}

/*
    Function Name: convertToASCII
    Description: this function converts a value to an array of characters to display it on the LCD
*/
static void convertToASCII(uint8*priceInASCII,uint8 priceValue)
{
    sint8 i;
    for(i=1;i >= 0 ;i--)
    {
        priceInASCII[i] = (priceValue % 10) + ASCII_VALUE_BASE;
        priceValue = priceValue /10;
    }
    priceInASCII[2]='\0';

}

/*
    Function Name: vTaskOrderFood
    Description: This task handles the ordering request from the user 
*/
void vTaskOrderFood(void *pvParameters)
{
    uint8 keyValue[3],confirmRequest; /*an array to store the keys selected and a variable to store the confirmation of the order*/
    uint8 price=0,priceInASCII[3];  /*Variable to handle the price and an array to handle its conversion to a string*/
    uint8 priceList[7]={50,60,65,15,20,10,10};
    uint8 orderNumber=1,orderInASCII[3];    /*A varible to store the order number and an array to store its conversion to a string*/
    while(1)
    {
        foodMissingCheck(foodOffset);   /*Checking for the missing food*/
        LCD_clearScreen();
        vTaskDelay(pdMS_TO_TICKS(200));
        LCD_displayString("Welcome...");    
        LCD_displayStringRowColumn(1,0,"Press 1");
        LCD_displayStringRowColumn(2,0,"To Order!");
        while(Keypad_getPressedKey() != 1)  /*Waiting for the user to press 1 to order*/
        {
            vTaskDelay(pdMS_TO_TICKS(500));
        }

        displayFood(MAIN);                      /*Displaying the main dish*/
        keyValue[0] = getChoice(MAIN_DISH_BASE);     /*getting the choice of the main dish*/
        price += priceList[keyValue[0]-1];      /*accumulating the result in the price variable to display it after finishing the order*/
        vTaskDelay(pdMS_TO_TICKS(1000));        /*delay for 1 second*/
        
        displayFood(SIDE);                      /*Displaying the side dish*/
        keyValue[1] = getChoice(SIDE_DISH_BASE);    /*getting the choice of the side dish*/
        price += priceList[keyValue[1] + 2];        /*accumulating the price*/
        vTaskDelay(pdMS_TO_TICKS(1000));            /*delay for 1 sec*/

        displayFood(DRINK);                         /*displaying the drinks*/
        keyValue[2] = getChoice(DRINK_BASE);        /*getting the choice*/
        price += priceList[6];                      /*adding the price*/
        vTaskDelay(pdMS_TO_TICKS(1000));            /*delay for 1 sec*/

        displayOrder(keyValue);                     /*Displaying the order to the user*/ 
        vTaskDelay(pdMS_TO_TICKS(1000));

        LCD_clearScreen();
        convertToASCII(priceInASCII,price);         /*Converting the price to a string to display it to the user*/
        LCD_displayStringRowColumn(0,0,"The price is ");
        LCD_displayString(priceInASCII);            /*displaying the price*/
        LCD_displayStringRowColumn(1,0,"Confirm  Cancel");  
        LCD_displayStringRowColumn(2,0,"   *       #");
        confirmRequest = Keypad_getPressedKey();        
        vTaskDelay(pdMS_TO_TICKS(500));
        while(confirmRequest != '*' && confirmRequest != '#')     /*getting the user confirmation or cancellation of the order*/
        {
            confirmRequest = Keypad_getPressedKey();
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        LCD_goToRowColumn(3,7);
        LCD_displayCharacter( confirmRequest );         /*Showing user's confirmation or cancellation*/
        vTaskDelay(pdMS_TO_TICKS(1000));
        LCD_clearScreen();
        if(confirmRequest == '*')       
        {
            LCD_displayStringRowColumn(0,0,"ORDER CONFIRMED");
            LCD_displayStringRowColumn(1,0,"Your Order");
            LCD_displayStringRowColumn(2,0,"number is ");
            convertToASCII(orderInASCII,orderNumber);           /*Converting the order number to a string to display it*/
            LCD_displayString(orderInASCII);

            if(orderNumber != 99)
            {
                orderNumber++;          /*The order number will overflow when it reaches 99*/
            }
            else
            {
                orderNumber = 1 ;
            }
            LCD_displayStringRowColumn(3,0,"guten Appetit!");   
            xQueueSend(Queue_sendOrderToOwner,(void *)keyValue,0);  /*Sending the order request to the queue to handle it to the owner*/
        }
        else if(confirmRequest == '#')
        {
            LCD_displayStringRowColumn(0,0,"ORDER CANCELLED");      
        }
        price=0;                /*resetting the price to get another order*/
        vTaskDelay(pdMS_TO_TICKS(1000));
        
    }
}
