/**
 * @file task_button_sw2.c
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_button_sw2.h"

SemaphoreHandle_t Sem_SW2;

/**
 * @brief
 * This task monitors SW1.  When SW1 is pressed, it update the color of the
 * stylus.  The new color of the stylus will be sent using a queue.
 * 
 * The color of the stylus should go from GREEN->ORANGE->RED->BLUE->GREEN...
 * @param pvParameters
 */
void task_button_sw2(void *pvParameters)
{
    /* Allocate any local variables used in this task */
    bool state_button_prev = false;
    bool state_button_curr = false;

    while (1)
    {
        /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
        vTaskDelay(50);

        /* Check the current state of the button */
        if ((PORT_BUTTONS->IN & SW2_MASK) == 0x00)
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
            xSemaphoreGive(Sem_SW2);
        }

        /* Update the previous button state */
        state_button_prev = state_button_curr;
    }
}
