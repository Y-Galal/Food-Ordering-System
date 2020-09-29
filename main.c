/**
 * Filename: main.c
 * description: Main code for the food ordering system project
*/
#include "imp_headers.h"



int main()
{

    systemInit();   /*Initializing all the drivers*/
    LCD_clearScreen();  /*Clearing the LCD for the first time*/

    /*  Task Name: Order Food 
        description: This task handles the ordering operation from the user
    */
    xTaskCreate(    vTaskOrderFood,         
                    "Order Food",
                    100,
                    NULL,
                    1,
                    &orderFoodHandle );

    /*
        Task Name: Request Order
        description: This task sends the order request and number to the owner to prepare the order.
    */
    xTaskCreate(    vTaskRequestOrder,
                    "Request Order",
                    100,
                    NULL,
                    2,
                    &requestOrderHandle );

    /*
        Task Name: God Mode
        Description: This task handles the communication between the owner and the system and secures this 
                    communication with a password.
    */
    xTaskCreate(    vTaskGodMode,
                    "God Mode",
                    100,
                    NULL,
                    3,
                    &godModeHandle );

    /*
        Queue Name: Queue_godModeRequest
        description: This queue saves the request sent from the pc to the system.
    */
    Queue_godModeRequest = xQueueCreate( 1, sizeof(uint8) );

    /**
     * Queue Name: Queue_sendOrderToOwner
     * description: this queue saves the order information and sends it from vTaskOrderFood to vTaskRequestOrder to send 
     * the order request to the owner.
    */
    Queue_sendOrderToOwner = xQueueCreate(1,3*sizeof(uint8));



    vTaskStartScheduler();      /*Starting the system*/
    while(1)    /*If everything is working properly, the system will not reach this line*/
    {

    }
}

