/**
 * @file task_uart_tx.c
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_uart_tx.h"

QueueHandle_t Queue_TX;

TaskHandle_t Task_UART_Tx;

/**
 * @brief task for handling tx
 * 
 * @param pvParameters 
 */
void task_uart_tx(void *pvParameters)
{   
    uint8_t highBits;
    uint8_t lowBits;
    uint8_t originalByte;
    char string[sizeof(uint8_t)+ 1];

    BaseType_t send_ready;
    while(1){
        // get transmit queue
        send_ready = xQueueReceive(Queue_TX, &originalByte, portMAX_DELAY);
        if(send_ready){
            string[0] = (char)originalByte;
            string[1] = '\0';
            remote_uart_tx_data_async(&string);  
        }
    }
}