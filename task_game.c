/**
 * @file task_game.c
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_game.h"

TaskHandle_t Task_Game;

/**
 * @brief 
 * This function will monitor queues, control game controls, and control drawn images, while state should be in game mode
 * 
 * @param pvParameters 
 */
void task_game(void *pvParameters)
{
    /* Allocate any local variables used in this task */
    //joystick
    BaseType_t joystick_status;
    joystick_position_t joystick_pos = JOYSTICK_POS_CENTER;
    
    //light
    BaseType_t light_status;
    u_int16_t light_val;
    uint16_t bgcolor;
    uint16_t wall_color;
    uint16_t bullet_bg_color;
    //gyro
    BaseType_t gyro_x_status;
    BaseType_t gyro_y_status;
    uint8_t gyro_x_val;
    uint8_t gyro_y_val;

    //switches
    BaseType_t sw1_status;
    BaseType_t sw2_status;

    Grid_Tile* curr_tile;

    int8_t mag = 0;
    tank_direction dir = HORIZONTAL;
    
    // message from other device
    uint8_t messageRX = 0;
    uint8_t moveTurrRX = 0;
    uint8_t moveTankRX = 0;
    uint8_t fireRX = 0;
    bool shouldMoveRX = false;
    bool shouldMoveTX = false;
    uint8_t moveTankTX = 0;
    uint8_t moveTurrTX = 0;
    uint8_t fireTX = 0;
    uint8_t messageTX = 0;
    BaseType_t recievedMESSAGE = pdFALSE;
    bool gyro_or_joystick = false;

    // Initialize tank properties
    Tank *p1Tank = (Tank *)malloc(sizeof(Tank));;
    p1Tank->x = 60;
    p1Tank->y = 60;
    p1Tank->health = 0;
    p1Tank->tread = false;
    p1Tank->color = LCD_COLOR_BLUE;
    p1Tank->tank_dir = VERTICAL;
    p1Tank->turr_dir = JOYSTICK_POS_UP;
    p1Tank->ID = 1;
    p1Tank->spawn_row = 5;
    p1Tank->spawn_col = 0;

    Tank *p2Tank = (Tank *)malloc(sizeof(Tank));;
    p2Tank->x = 240;
    p2Tank->y = 60;
    p2Tank->health = 0;
    p2Tank->tread = false;
    p2Tank->color = LCD_COLOR_RED;
    p2Tank->tank_dir = VERTICAL;
    p2Tank->turr_dir = JOYSTICK_POS_UP;
    p1Tank->ID = 2;
    p2Tank->spawn_row = 0;
    p2Tank->spawn_col = 7;

    /*allocate memory for bullets*/
    Bullet* p1bullet = (Bullet *)malloc(sizeof(Bullet));
    p1bullet->num_bounce = 0;

    Bullet* p2bullet = (Bullet *)malloc(sizeof(Bullet));
    p2bullet->num_bounce = 0;

    Bullet* myBullet;
    Bullet* opBullet;

    // Initialize the Battle Field Grid
    Grid_Tile* battlefield= malloc(6 * 8 * sizeof(Grid_Tile)); // r by c battlefield[i * c + j] accesses (i, j)
    // Check if allocation was successful
    if (battlefield == NULL) {
        CY_ASSERT(0);
    }
    init_battlefield(battlefield);
    
    //get light val and make appropriate color scheme
    light_status = xQueueReceive(Queue_Light, &light_val, 5);
    if(light_status == pdTRUE){
        if(light_val < 15){//if light sensor returns a low light intensity, go to dark mode
            bgcolor = LCD_COLOR_BLACK;
            wall_color = LCD_COLOR_ORANGE;
            bullet_bg_color = LCD_COLOR_BLACK;
        }else{//light mode
            bgcolor = LCD_COLOR_GREEN;
            wall_color = LCD_COLOR_ORANGE;
            bullet_bg_color = LCD_COLOR_GREEN;
        }

    }
    lcd_clear_screen(bgcolor);
    draw_battlefield(battlefield, wall_color);

    // init led vars 
    uint8_t active_led = 0x7c;
    uint8_t io_sw = 0xFF;

    /* Start timer */
    timer_init(&hw03_timer_obj, &hw03_timer_cfg, 10000000, Timer_Handler);

    /* Get Player Choice from Queue */
    uint8_t playerNum = 1;
    xQueueReceive(Queue_Player_Choice, &playerNum, portMAX_DELAY);
    Tank *myTank;
    Tank *opTank;
    if(playerNum == 1){ // I'm player 1
        myTank = p1Tank;
        myBullet = p1bullet;
        opTank = p2Tank;
        opBullet = p2bullet;
    }else{
        // else player 2
        myTank = p2Tank;
        myBullet = p2bullet;
        opTank = p1Tank;
        opBullet = p1bullet;
    }

    while (1)
    {
        /* Need to read the IO Expander due to the way Interrupts from the IO Expander work */
        io_sw = io_expander_get_input_port();
        /* Set the IO Expander Outputs to the active LED */ 
	    io_expander_set_output_port(active_led); 
        // reset RX signal
        messageRX = 0;

        // reset transmit signal
        messageTX = 0x00;
        moveTurrTX = 0;
        shouldMoveTX = false;
        moveTankTX = 0;
        fireTX = 0;
        messageTX = 0;
        // Check if Tank is dead health = 0;
        if(myTank->health < 1){
            // make draw over current tile
            lcd_draw_rectangle_centered(myTank->x, myTank->y, 34, 34, bgcolor);
            // free current tile
            curr_tile = ((Grid_Tile * )&battlefield[8*(myTank->x/40) + (myTank->y/40)]);
            curr_tile->occupied = false;
            // set location to bottom right grid
            curr_tile = ((Grid_Tile * )&battlefield[8*myTank->spawn_row + myTank->spawn_col]);
            myTank->x = curr_tile->x;
            myTank->y = curr_tile->y;
            curr_tile->claimed = myTank->ID;
            curr_tile->occupied = true;
            draw_tank(myTank, bgcolor);
            myTank->health = 5;
            // Update Score
            if(opTank == p1Tank){
                xTaskNotify(Task_Save_Eeprom, 2, eSetValueWithOverwrite);
            }else{
                xTaskNotify(Task_Save_Eeprom, 1, eSetValueWithOverwrite);
            }
        }

        if(opTank->health < 1){
            // make draw over current tile
            lcd_draw_rectangle_centered(opTank->x, opTank->y, 34, 34, bgcolor);
            // free current tile
            curr_tile = ((Grid_Tile * )&battlefield[8*(opTank->x/40) + (opTank->y/40)]);
            curr_tile->occupied = false;
            // set location to bottom right grid
            curr_tile = ((Grid_Tile * )&battlefield[8*opTank->spawn_row + opTank->spawn_col]);
            opTank->x = curr_tile->x;
            opTank->y = curr_tile->y;
            curr_tile->claimed = opTank->ID;
            curr_tile->occupied = true;
            draw_tank(opTank, bgcolor);
            opTank->health = 5;
            // Update Score
            if(myTank == p1Tank){
                xTaskNotify(Task_Save_Eeprom, 2, eSetValueWithOverwrite);
            }else{
                xTaskNotify(Task_Save_Eeprom, 1, eSetValueWithOverwrite);
            }
        }

        /* Receive the current position of the joystick from the 
         * Timeout after 5mS. 
        */
        joystick_status = xQueueReceive(Queue_Joystick_Position, &joystick_pos, 5);

        /* Check to see if a valid message was received */
        if (joystick_status == pdTRUE)
        {
            myTank->turr_dir = joystick_pos;
            draw_turr(myTank);
        }
          /*alternate between drive mode: gyro = sw1 true, joystick = sw1 false*/
        sw1_status = xSemaphoreTake(Sem_SW1, 5);
        if(sw1_status == pdTRUE){
            gyro_or_joystick = !gyro_or_joystick;
        }
        if(gyro_or_joystick){
            
            /* Get tank movement from Accelerometer */
            gyro_x_status = xQueueReceive(Queue_Gyro_X_Position, &gyro_x_val, 5);
            if(gyro_x_status == pdTRUE){
                /* Update tank Position */
                //user tilts to left
                if((gyro_x_val >=25) && (gyro_x_val < 40)){ //slight tilt
                        dir = HORIZONTAL;
                        mag = -1;
                        moveTankTX = 0;
                        shouldMoveTX = true;
                }
                if((gyro_x_val >=40) && (gyro_x_val < 55)){//full tilt
                        dir = HORIZONTAL;
                        mag = -2;
                        moveTankTX = 1;
                        shouldMoveTX = true;
                }
                //user tilts right
                if((gyro_x_val >=205) && (gyro_x_val < 215)){ //slight tilt
                        dir = HORIZONTAL;
                        mag = 2;
                        moveTankTX = 3;
                        shouldMoveTX = true;
                }
                if((gyro_x_val >=215) && (gyro_x_val < 235)){//full tilt
                        dir = HORIZONTAL;
                        mag = 1;
                        moveTankTX = 2;
                        shouldMoveTX = true;
                }
        }

        
        gyro_y_status = xQueueReceive(Queue_Gyro_Y_Position, &gyro_y_val, 5);
        if(gyro_y_status == pdTRUE){
            //player tilts down (screen towards user)
            if((gyro_y_val >=45) && (gyro_y_val < 55)){//sight
                    dir = VERTICAL;
                    mag = 1;
                    moveTankTX = 6;
                    shouldMoveTX = true;
            }
            if((gyro_y_val >=55) && (gyro_y_val < 65)){//full tilt
                    dir = VERTICAL;
                    mag = 2;
                    moveTankTX = 7;
                    shouldMoveTX = true;
            }
            //user tilts up, (screen away from user)
            if(((gyro_y_val >0) && (gyro_y_val < 5)) || ((gyro_y_val >240) && (gyro_y_val < 255))){//full tilt
                    dir = VERTICAL;
                    mag = -2;
                    moveTankTX = 5;
                    shouldMoveTX = true;
            }
            if((gyro_y_val >=5) && (gyro_y_val < 15)){
                    dir = VERTICAL;
                    mag = -1;
                    moveTankTX = 4;
                    shouldMoveTX = true;
            }
            //gyro in center
            if((gyro_x_val >=0) && (gyro_x_val < 25) && (gyro_y_val >=15) && (gyro_y_val < 45)){
                dir = HORIZONTAL;
                mag = 0;
            }
            move_tank_in_Tile(myTank, battlefield, dir, mag, bgcolor);
        }
        }else{
            if(joystick_status == pdTRUE){
                switch (joystick_pos)
                    {
                    case JOYSTICK_POS_UP:
                        dir = VERTICAL;
                        mag = -1;
                        shouldMoveTX = true;
                        moveTankTX = 4;
                        break;
                    case JOYSTICK_POS_DOWN:
                        dir = VERTICAL;
                        mag = 1;
                        shouldMoveTX = true;
                        moveTankTX = 6;
                        break;
                    case JOYSTICK_POS_LEFT:
                        dir = HORIZONTAL;
                        mag = -1;
                        shouldMoveTX = true;
                        moveTankTX = 0;
                        break;
                    case JOYSTICK_POS_RIGHT:
                        dir = HORIZONTAL;
                        mag = 1;
                        shouldMoveTX = true;
                        moveTankTX = 2;
                        break;
                    default:
                        mag = 0;
                        break;
                }
                move_tank_in_Tile(myTank, battlefield, dir, mag, bgcolor);
            }
        }

        /* check sw2 */
        sw2_status = xSemaphoreTake(Sem_SW2, 5);
        if(sw2_status == pdTRUE){
            if(p1bullet->num_bounce == 0){ //prevent multiple attempts at accessing the same variable
                shoot_bullet(myTank, p1bullet);
                fireTX = 1;
            }
        }
        
        /* Transmit our tank data */
        // update transmit signal
        switch(myTank->turr_dir){
            case JOYSTICK_POS_LEFT:
                moveTurrTX = 4;
                break;
            case JOYSTICK_POS_RIGHT:
                moveTurrTX = 3;
                break;
            case JOYSTICK_POS_UP:
                moveTurrTX = 0;
                break;
            case JOYSTICK_POS_DOWN:
                moveTurrTX = 5;
                break;
            case JOYSTICK_POS_UPPER_LEFT:
                moveTurrTX = 2;
                break;
            case JOYSTICK_POS_UPPER_RIGHT:
                moveTurrTX = 1;
                break;
            case JOYSTICK_POS_LOWER_LEFT:
                moveTurrTX = 7;
                break;
            case JOYSTICK_POS_LOWER_RIGHT:
                moveTurrTX = 6;
                break;
        }

        if(shouldMoveTX){
            messageTX = ((0x80) + (moveTankTX << 4) + (moveTurrTX << 1) + fireTX);
        }else{
            messageTX = ((moveTurrTX << 1) + fireTX);
        }
        if(messageTX != 0){
            xQueueSend(Queue_TX, &messageTX, 5);
        }

        //if bullet generated, call move func
        if(p1bullet->num_bounce != 0){
            move_bullet(p1bullet, battlefield, bullet_bg_color);
            //check hit regs only if bullet is moving
            if(bullet_hit(p1bullet, myTank, bullet_bg_color)){
                myTank->health -= 1;
                // update LEDs
                /* Rotate the active LED */
                if(active_led == 0xc0)
                {
                    active_led = 0x7c;
                }
                else
                {
                    active_led = (active_led << 1 );
                }
            }
            if(bullet_hit(p1bullet, opTank, bullet_bg_color)){
                opTank->health -= 1;
            }
        }
        /* Check Op Bullet */
        if(p2bullet->num_bounce != 0){
            move_bullet(p2bullet, battlefield, bullet_bg_color);
            //check hit regs only if bullet is moving
            if(bullet_hit(p2bullet, myTank, bullet_bg_color)){
                myTank->health -= 1;
            }
            if(bullet_hit(p2bullet, opTank, bullet_bg_color)){
                opTank->health -= 1;
            }
        }

        /* Check for if the other player info is ready */
        // get signal from RX

        //signals here are mixed up, moves down and left or something, check message value as well as 
        recievedMESSAGE = xQueueReceive(Queue_RX, &messageRX, 5);
        if(recievedMESSAGE == pdTRUE){
            /* Move Op Tank */
            shouldMoveRX = messageRX & 0x80;
            if(shouldMoveRX != 0){
                moveTankRX = (messageRX & 0x70) >> 4;
                switch(moveTankRX){
                    case 0:
                        move_tank_in_Tile(opTank, battlefield, HORIZONTAL, -1, bgcolor);
                        break;
                    case 1:
                        move_tank_in_Tile(opTank, battlefield, HORIZONTAL, -2, bgcolor);
                        break;
                    case 2:
                        move_tank_in_Tile(opTank, battlefield, HORIZONTAL, 1, bgcolor);
                        break;
                    case 3:
                        move_tank_in_Tile(opTank, battlefield, HORIZONTAL, 2, bgcolor);
                        break;
                    case 4:
                        move_tank_in_Tile(opTank, battlefield, VERTICAL, -1, bgcolor);
                        break;
                    case 5:
                        move_tank_in_Tile(opTank, battlefield, VERTICAL, -2, bgcolor);
                        break;
                    case 6: 
                        move_tank_in_Tile(opTank, battlefield, VERTICAL, 1, bgcolor);
                        break;
                    case 7:
                        move_tank_in_Tile(opTank, battlefield, VERTICAL, 2, bgcolor);
                    break;
                }

            }
            /* Move Op Turret */
            //check this statement and message rx, maybe logic in here is wrong.
            //current problem is that joystick pos is at center
            uint8_t moveTurrRX = (messageRX & 0x0E) >> 1;
            switch(moveTurrRX){
                case 0:
                    opTank->turr_dir = JOYSTICK_POS_UP;
                    break;
                case 1:
                    opTank->turr_dir = JOYSTICK_POS_UPPER_RIGHT;
                    break;
                case 2:
                    opTank->turr_dir = JOYSTICK_POS_UPPER_LEFT;
                    break;
                case 3:
                    opTank->turr_dir = JOYSTICK_POS_RIGHT;
                    break;
                case 4:
                    opTank->turr_dir = JOYSTICK_POS_LEFT;
                    break;
                case 5:
                    opTank->turr_dir = JOYSTICK_POS_DOWN;
                    break;
                case 6: 
                    opTank->turr_dir = JOYSTICK_POS_LOWER_RIGHT;
                    break;
                case 7:
                    opTank->turr_dir = JOYSTICK_POS_LOWER_LEFT;
                break;
            }
            draw_turr(opTank);
            /* Fire Op Tank */
            fireRX = (messageRX & 0x01);
            if ( fireRX != 0)
            {
                if(opBullet->num_bounce == 0) //prevent multiple attempts at accessing the same variable
                    shoot_bullet(opTank, opBullet);
            }
        }
    }
    //Free memory
    free(p1Tank);
    free(p2Tank);
    free(p1bullet);
    free(p2bullet);
    free(battlefield);
}


