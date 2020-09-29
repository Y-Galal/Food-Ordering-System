#include "imp_headers.h"

extern const uint8 *food[9];        /*getting the food names to send them to the owner*/


static void decrementFood(uint8 foodChoice,sint8 dishType);

/*
    Function Name: vTaskRequestOrder
    Description: this task decrements the food saved in the EEPROM and sends the order request and the number to the owner 
*/
void vTaskRequestOrder(void *pvParameters)
{
    uint8 orderContents[3];  uint8 orderNumberToOwner=1;
    while(1)
    {
        xQueueReceive(Queue_sendOrderToOwner,(void *)&orderContents,portMAX_DELAY); /*waiting to receive the order contents from ordering task*/
        UART_IntDisable();
        /*Displaying the order contents to the owner*/
        UART_sendString(food[orderContents[0] + MAIN_DISH_BASE ]);
        UART_sendString(food[orderContents[1] + SIDE_DISH_BASE]);
        UART_sendString(food[orderContents[2] + DRINK_BASE]);
        UART_sendNumber(orderNumberToOwner);    /*sending the order number*/
        if(orderNumberToOwner != 99)
        {
            orderNumberToOwner++;
        }
        else
        {
            orderNumberToOwner = 1 ;
        }
        UART_IntEnable();

        /*decrementing the order contents from the quantity saved in the EEPROM*/
        decrementFood(orderContents[0],MAIN_DISH_BASE); 
        decrementFood(orderContents[1],SIDE_DISH_BASE);
        decrementFood(orderContents[2],DRINK_BASE);
    }
}

/*
    Function Name: decrementFood
    Description: this function decrements the food in the EEPROM and 
                    if the quantity of food reaches 0, it will send a notification to the owner
*/
static void decrementFood(uint8 foodChoice,sint8 dishType)
{
    sint32 foodQuantity;
    EEPROM_readData(MEMORY_BLOCK, FOOD_QUANTITY_ADD + foodChoice + dishType, (uint32*)&foodQuantity);   
    if(foodQuantity > 0)
    {
        EEPROM_writeData(MEMORY_BLOCK, FOOD_QUANTITY_ADD + foodChoice + dishType, foodQuantity-1);
    }
    if(foodQuantity == 1)
    {
        UART_sendString("Food in menu is missing, add more of:");
        UART_sendString(food[foodChoice+dishType]);
    }
}
