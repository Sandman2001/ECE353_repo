/**
 * @file task_start.h
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"

#ifndef __TASK_START_H__
#define __TASK_START_H__

extern QueueHandle_t Queue_Player_Choice;
extern QueueHandle_t Queue_TX;
extern QueueHandle_t Queue_RX;
extern void task_start(void *pvParameters);

#endif