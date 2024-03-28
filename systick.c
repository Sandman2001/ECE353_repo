/**
 * @file systick.c
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "systick.h"

/**
 * @brief 
 *  Initilaizes the SysTick Timer for a configurable interval of time. 
 * @param ticks 
 *  The Number clock cycles 
 */
void systick_init(uint32_t ticks)
{
    if(ticks > 0xFFFFFF){
        // SysTick Timer is only 24 bits, so return an error
        return false;
    }
    // Turn off the SysTick Timer, remember when changing device first shut off
    SysTick->CTRL = 0;

    // Set the current value to 0
    SysTick->VAL = 0;

    // Set the period in this case the number of ticks
    SysTick->LOAD = ticks - 1; // count down to 0, so 100 to done is tick numbers: 99 to 0

    // Set the Clock source to the main CPU clock and turn the time on
    SysTick->CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_ENABLE;
    return true;
}


bool systick_expired(void)
{
    if(SysTick->CTRL & SYSTICK_CTRL_COUNTFLAG){
        // check if equal to non-zero, that means it expired
        return true;
    }else{
        // else still counting
        return false;
    }
   
}