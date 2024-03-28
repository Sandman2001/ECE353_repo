/**
 * @file task_uart_tx.h
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"

#ifndef __TASK_UART_TX_H__
#define __TASK_UART_TX_H__

extern TaskHandle_t Task_UART_Tx;
extern void task_uart_tx(void *pvParameters);

#endif