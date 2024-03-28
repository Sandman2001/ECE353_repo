/**
 * @file task_save_eeprom.h
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"

#ifndef __TASK_SAVE_EEMPROM_H__
#define __TASK_SAVE_EEPROM_H__

extern TaskHandle_t Task_Save_Eeprom;
extern void task_save_eeprom(void *pvParameters);

#endif