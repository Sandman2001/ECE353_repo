/**
 * @file task_uart_rx.h
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"

#ifndef __TASK_UART_RX_H__
#define __TASK_UART_RX_H__

extern TaskHandle_t Task_UART_Rx;
extern void task_uart_rx(void *pvParameters);void task_gyro(void *pvParameters);
#endif