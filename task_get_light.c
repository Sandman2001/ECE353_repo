/**
 * @file task_get_light.c
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_get_light.h"

QueueHandle_t Queue_Light;

/**
 * @brief 
 * This function will retreive the light value, send to a queue, then delete itself
 * 
 * @param pvParameters 
 */
void task_get_light(void *pvParameters){
    uint32_t regval;
    uint16_t ch1_val;

    regval = LTR_329_get_light(); //get light val
    ch1_val = (uint16_t)((0xFF) & regval); 
    xQueueSend(Queue_Light, &ch1_val, portMAX_DELAY);
    vTaskDelete(NULL); //delete this task

}