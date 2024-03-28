/**
 * @file task_uart_rx.c
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_uart_rx.h"

QueueHandle_t Queue_RX;
TaskHandle_t Task_UART_Rx;

/**
 * @brief Task for handling rx messages
 * 
 * @param pvParameters 
 */
void task_uart_rx(void *pvParameters){
    char remote_rx_message[2] = { 0, 0 };
    uint8_t message;
    while(1){
        /* Initialize the array to all 0*/
        vTaskDelay(5);
        if (remote_uart_rx_data_async(remote_rx_message, 2))
        {   
            message = (int)remote_rx_message[0];
            // add message to the queue to be handled
            xQueueSend(Queue_RX, &message, portMAX_DELAY);
        }
    }
}