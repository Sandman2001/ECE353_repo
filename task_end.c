/**
 * @file task_end.c
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_end.h"

TaskHandle_t Task_End;

/**
 * @brief 
 * This task allows for the device to display who won and to allow the user to wait for other user to ready up and play again
 * @param pvParameters 
 */
void task_end(void *pvParameters)
{
    /* Allocate any local variables used in this task */
    Tank *tankClouds = (Tank *)malloc(sizeof(Tank));;
    tankClouds->x = 240;
    tankClouds->y = 60;
    tankClouds->health = 5;
    tankClouds->tread = false;
    tankClouds->color = LCD_COLOR_MAGENTA;
    tankClouds->tank_dir = VERTICAL;
    tankClouds->turr_dir = JOYSTICK_POS_UP;

    uint8_t Winner = 0;
    uint8_t alltimebest_red = 0;
    uint8_t alltimebest_blue = 0;
    uint8_t newalltimebest_red = 0;
    uint8_t newalltimebest_blue = 0;

    BaseType_t sw2_status = pdFALSE;
    while (1)
    {
        // load previous all time best
        alltimebest_blue = eeprom_read_byte(0x00);
        alltimebest_red = eeprom_read_byte(0x01);
        /* Check to see if timer is done */
        ulTaskNotifyTake(true, portMAX_DELAY);
        cyhal_timer_reset(&hw03_timer_obj);
        cyhal_timer_stop(&hw03_timer_obj);

        /* Delete Game Task */
        vTaskDelete(Task_Game);
        lcd_clear_screen(LCD_COLOR_CYAN);

        /* Display score */
        // Notify the score-keeping task that the game is finished
        xTaskNotify(Task_Save_Eeprom, 3, eSetValueWithOverwrite);

        // Wait for results
        xQueueReceive(Queue_Player_Win, &Winner, portMAX_DELAY);

        // check if player 1 won else player 2 won
        if(Winner == 1){ // if p1 won then Blue won
            lcd_draw_image(SCREEN_CENTER_COL-75, SCREEN_CENTER_ROW-50, WinLogoWidthPixels, WinLogoHeightPixels, BlueWinsBitmaps, LCD_COLOR_BLUE, LCD_COLOR_CYAN);
        }else if(Winner == 2){ // if p2 won then Red won
            lcd_draw_image(SCREEN_CENTER_COL-75, SCREEN_CENTER_ROW-50, WinLogoWidthPixels, WinLogoHeightPixels, RedWinsBitmaps, LCD_COLOR_RED, LCD_COLOR_CYAN);
        }else{ // if a tie display tie
            lcd_draw_image(SCREEN_CENTER_COL-75, SCREEN_CENTER_ROW-50, WinLogoWidthPixels, WinLogoHeightPixels, TieWinsBitmaps, LCD_COLOR_BLACK, LCD_COLOR_CYAN);
        }

        // check if all time best
        newalltimebest_blue = eeprom_read_byte(0x00);
        if(newalltimebest_blue != alltimebest_blue){
            lcd_draw_image(SCREEN_CENTER_COL-75, SCREEN_CENTER_ROW, WinLogoWidthPixels, WinLogoHeightPixels, atpblue_map, LCD_COLOR_BLUE, LCD_COLOR_CYAN);
        }
        newalltimebest_red = eeprom_read_byte(0x01);
        if(newalltimebest_red != alltimebest_red){
            lcd_draw_image(SCREEN_CENTER_COL-75, SCREEN_CENTER_ROW+50, WinLogoWidthPixels, WinLogoHeightPixels, atpred_map, LCD_COLOR_RED, LCD_COLOR_CYAN);
        }
        
        // wait for s2 to be pressed
        
        while(sw2_status != pdTRUE){
            // move to wait screen "start task"
            move_tank(tankClouds, VERTICAL, -1, LCD_COLOR_WHITE);
            sw2_status = xSemaphoreTake(Sem_SW2, 5);
            vTaskDelay(5);
        }
        // start the start task
            xTaskCreate(
            task_start, // impliments task
            "Start Screen Task", // idenifier
            configMINIMAL_STACK_SIZE, // stack size
            NULL, // passed in parameters
            3, // priority (1 is lowest (idle), higher is more important )
            NULL // task handle for notifications
            );
    }
}
