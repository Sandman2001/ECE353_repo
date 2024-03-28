/**
 * @file task_start.c
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_start.h"

/**
 * @brief 
 * This task allows for the syncing of devices to communicate, select players, and get ready to play the game
 * @param pvParameters 
 */
void task_start(void *pvParameters)
{
    /* Allocate any local variables used in this task */
    Tank *p1Tank = (Tank *)malloc(sizeof(Tank));;
    p1Tank->x = 60;
    p1Tank->y = 60;
    p1Tank->health = 5;
    p1Tank->tread = false;
    p1Tank->color = LCD_COLOR_BLUE;
    p1Tank->tank_dir = VERTICAL;
    p1Tank->turr_dir = JOYSTICK_POS_UP;

    Tank *p2Tank = (Tank *)malloc(sizeof(Tank));;
    p2Tank->x = 240;
    p2Tank->y = 60;
    p2Tank->health = 5;
    p2Tank->tread = false;
    p2Tank->color = LCD_COLOR_RED;
    p2Tank->tank_dir = VERTICAL;
    p2Tank->turr_dir = JOYSTICK_POS_UP;
    // check for sw2 pressed
    BaseType_t sw2_status;
    bool opReady = false;
    uint8_t message = 0;
    uint8_t readymsg = 0xFF;
    /* Set up Screen */
    lcd_clear_screen(LCD_COLOR_GREEN);

    lcd_draw_image((SCREEN_X / 3) - 5 , 120, tankLogoWidthPixels, tankLogoHeightPixels, tanksLogobitmap, LCD_COLOR_BLACK, LCD_COLOR_GREEN);

    while (1)
    {
        /*base state, no player has pressed s1 yet*/
        move_tank(p1Tank, HORIZONTAL, -1, LCD_COLOR_BROWN);
        move_tank(p2Tank, VERTICAL, -1, LCD_COLOR_BROWN);

        sw2_status = xSemaphoreTake(Sem_SW2, 5);
        if(sw2_status){//board assumes P1
            // Wait for acknowledgment from Player 2 (0xF0)
            lcd_wait_for_other_player();
            // check if receive 
            if(xQueueReceive(Queue_RX, &message, 5) == pdTRUE){
                opReady = true;
            }
            // transmit start
            xQueueSend(Queue_TX, &readymsg, portMAX_DELAY);
            // wait for other player if not ready already
            if(!opReady){
                xQueueReceive(Queue_RX, &message, portMAX_DELAY);
            }
            // check if first player
            if(opReady){
                // queue p2
                xQueueSend(Queue_Player_Choice, 2, portMAX_DELAY);
            }else{
                // queue p1 choice
                xQueueSend(Queue_Player_Choice, 1, portMAX_DELAY);
            }
            
            /* TODO AFTER RECIEVING ACK FROM OTHER PLAYER MOVE TO GAME MODE*/
            /* Create Game Task */
            xTaskCreate(
                task_game, // impliments task
                "Game Screen Task", // idenifier
                configMINIMAL_STACK_SIZE, // stack size
                NULL, // passed in parameters
                2, // priority (1 is lowest (idle), higher is more important )
                &Task_Game // task handle for notifications
            );

            /*get light levels for game*/
            xTaskCreate(
                task_get_light, // impliments task
                "get light levels", // idenifier
                configMINIMAL_STACK_SIZE, // stack size
                NULL, // passed in parameters
                2, // priority (1 is lowest (idle), higher is more important )
                NULL // task handle for notifications
            );
            free(p1Tank);
            free(p2Tank);
            /* Delete This Task */
            vTaskDelete(NULL);
        }   
    }
}
