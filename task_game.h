/**
 * @file task_game.h
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"

#ifndef __TASK_GAME_H__
#define __TASK_GAME_H__

extern QueueHandle_t Queue_Gyro_Y_Position;
extern QueueHandle_t Queue_Gyro_X_Position;
extern QueueHandle_t Queue_Light;
extern TaskHandle_t Task_Game;

extern void task_game(void *pvParameters);

#endif