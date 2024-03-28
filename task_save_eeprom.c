/**
 * @file task_save_eeprom.c
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_save_eeprom.h"

QueueHandle_t Queue_Player_Choice;
QueueHandle_t Queue_Player_Win;

TaskHandle_t Task_Save_Eeprom;


/**
 * @brief
 * This task saves value from the queue to eemprom and tracks score for the game
 * 
 * @param pvParameters
 */
void task_save_eeprom(void *pvParameters)
{
    /* Allocate any local variables used in this task */
    uint32_t ulNotificationValue;
    BaseType_t p1_status = false;
    BaseType_t p2_status = false;

    uint8_t p1_score = 0;
    uint8_t p2_score = 0;

    uint32_t all_time_best_p1 = eeprom_read_byte(0x00);
    uint32_t all_time_best_p2 = eeprom_read_byte(0x01);

    // get previous high scores
    while (1)
    {
        // Wait for a notification
        if (xTaskNotifyWait(0x00, 0xffffffff, &ulNotificationValue, portMAX_DELAY) == pdTRUE)
        {
            switch (ulNotificationValue)
            {
                case 1:
                    // Increment Player 1's score
                    p1_score++;
                    break;
                case 2:
                    // Increment Player 2's score
                    p2_score++;
                    break;
                case 3:
                    // Game finished, process and send the score
                    // if larger than the EEPROM values: then save to EEPROM
                    if(p1_score > all_time_best_p1){
                        eeprom_write_byte( 0x00, p1_score);
                    }
                    if(p2_score > all_time_best_p2){
                        eeprom_write_byte( 0x01, p2_score);
                    }
                    // check who one and send signal to the final task
                    if(p1_score == p2_score){
                        xQueueSend(Queue_Player_Win, (uint8_t) 0, portMAX_DELAY);
                    }else if(p1_score > p2_score){
                        xQueueSend(Queue_Player_Win, (uint8_t) 1, portMAX_DELAY);
                    }else{
                        xQueueSend(Queue_Player_Win, (uint8_t) 2, portMAX_DELAY);
                    }
                    p1_score = 0;
                    p2_score = 0;
                    break;
            }
        }
    }
}