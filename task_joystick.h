/**
 * @file task_joystick.h
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"

#ifndef __TASK_JOYSTICK_H__
#define __TASK_JOYSTICK_H__

extern QueueHandle_t Queue_Joystick_Position;
extern void task_joystick(void *pvParameters);

#endif