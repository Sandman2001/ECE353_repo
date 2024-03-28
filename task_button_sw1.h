/**
 * @file task_button_sw1.h
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"

#ifndef __TASK_BUTTON_SW1_H__
#define __TASK_BUTTON_SW1_H__

extern SemaphoreHandle_t Sem_SW1;

extern void task_button_sw1(void *pvParameters);

#endif