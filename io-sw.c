/**
 * @file io-sw.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "io-sw.h"

/*****************************************************************************/
/* ICE 02 START */
/*****************************************************************************/
void push_buttons_init(void)
{
    cy_rslt_t rslt;
    rslt = cyhal_gpio_init(PIN_SW1, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, 0);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
    rslt = cyhal_gpio_init(PIN_SW2, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, 0);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
    rslt = cyhal_gpio_init(PIN_SW3, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, 0);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);
} 
/*****************************************************************************/
/* ICE 02 END */
/*****************************************************************************/


/*****************************************************************************/
/* ICE 03 START */
/*****************************************************************************/
/**
 * @brief
 * Returns which button has been pressed and then released. 
 *  
 * This function should examine the current state of each button and
 * compare it with the previous state of the button.  If the current
 * state is not-pressed and the previous state was pressed, return that
 * the button has been released.
 * 
 * If none of the buttons have been released, check to see if the button is 
 * currently being pressed.  If it is, return that the button is being pressed
 * 
 * If multiple buttons are pressed/released, return the button state of the button
 * with the highest SW number (SW3 has highest priority, then SW2, then SW1)
 * 
 * This function should only read the IN register once.
 * 
 * @return button_state_t 
 * Returns which of the buttons is currently being pressed.  
 */
button_state_t get_buttons(void)
{ 
    static bool sw1_curr = false;
    static bool sw1_prev = false;
    static bool sw2_curr = false;
    static bool sw2_prev = false;
    static bool sw3_curr = false;
    static bool sw3_prev = false;
    
    uint32_t regval = PORT_BUTTONS -> IN;

    // set previous to past currents
    sw3_prev = sw3_curr;
    sw2_prev = sw2_curr;
    sw1_prev = sw1_curr;

    // update current and return if current is pressed
    if((regval & SW3_MASK) == 0){
        // check if button SW3 is pressed
        sw3_curr = true;
        sw2_curr = false;
        sw1_curr = false;
        return BUTTON_SW3_PRESSED;
    }else if((regval & SW2_MASK) == 0){
        // check if button SW2 is pressed
        sw3_curr = false;
        sw2_curr = true;
        sw1_curr = false;
        return BUTTON_SW2_PRESSED;
    }else if((regval & SW1_MASK) == 0){
        // check if button SW1
        sw3_curr = false;
        sw2_curr = false;
        sw1_curr = true;
        return BUTTON_SW1_PRESSED;
    }else{ 
        // no buttons pressed
        sw3_curr = false;
        sw2_curr = false;
        sw1_curr = false;
    }

    // check previous state and compare to current, if current is false and prev is true then return released
    if(!sw3_curr && sw3_prev){
        //check if previous sw 3 state was true and now false
        return BUTTON_SW3_RELEASED;
    }else if(!sw2_curr && sw2_prev){
        //check if previous sw2 state was true and now false
        return BUTTON_SW2_RELEASED;
    }else if(!sw1_curr && sw1_prev){
        //check if previous sw1 state was true and now false
        return BUTTON_SW1_RELEASED;
    }else{
        return BUTTON_INACTIVE;
    }
    
}

/*****************************************************************************/
/* ICE 03 END */
/*****************************************************************************/