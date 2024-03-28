/**
 * @file task_end.h
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"

#ifndef __TASK_END_H__
#define __TASK_END_H__

extern QueueHandle_t Queue_Player_Win;
extern TaskHandle_t Task_End;
extern void task_end(void *pvParameters);

#endif