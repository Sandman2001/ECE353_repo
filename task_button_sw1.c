/**
 * @file task_button_sw1.c
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_button_sw1.h"
SemaphoreHandle_t Sem_SW1;

/**
 * @brief
 * This task monitors SW1.  When SW1 is pressed, it update the bool in the queue
 * @param pvParameters
 */
void task_button_sw1(void *pvParameters)
{
    /* Allocate any local variables used in this task */
    bool state_button_prev = false;
    bool state_button_curr = false;

    while (1)
    {
        /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
        vTaskDelay(50);

        /* Check the current state of the button */
        if ((PORT_BUTTONS->IN & SW1_MASK) == 0x00)
        {
            state_button_curr = true;
        }
        else
        {
            state_button_curr = false;
        }

        /* If the button was pressed, update queue
         */
        if (state_button_curr && !state_button_prev)
        {
            /* Send the bool to queue
             */
            xSemaphoreGive(Sem_SW1);
        }

        /* Update the previous button state */
        state_button_prev = state_button_curr;
    }
}
