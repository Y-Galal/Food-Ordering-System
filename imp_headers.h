#ifndef IMP_HEADERS_H
#define IMP_HEADERS_H


/*For RTOS*/
#include <stdint.h>
#include <stdbool.h>
#include "OS_Files/FreeRTOS.h"
#include "OS_Files/task.h"
#include "OS_Files/semphr.h"

#include "init.h"

#define ASCII_VALUE_BASE '0'        /*Base value for ascii numbers to convert them to an actual value*/

#define MAIN_DISH_BASE -1           
#define SIDE_DISH_BASE 2
#define DRINK_BASE     5

#define FOOD_QUANTITY_ADD 0
#define MEMORY_BLOCK 0


TaskHandle_t orderFoodHandle,godModeHandle,requestOrderHandle;
QueueHandle_t Queue_sendOrderToOwner;
QueueHandle_t Queue_godModeRequest;

void vTaskRequestOrder(void *pvParameters);
void vTaskOrderFood(void *pvParameters);
void vTaskGodMode(void *pvParameters);



#endif

