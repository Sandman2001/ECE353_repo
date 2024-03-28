/**
 * @file hw03.c
 * @author Zach Gunderson and Andrew Sanders (ztgunderson@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "main.h"
#include "hw03.h"

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/

/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
char HW03_DESCRIPTION[] = "ECE353: HW03 Zachary Gunderson, Andrew Sanders";

cyhal_timer_t hw03_timer_obj;
cyhal_timer_cfg_t hw03_timer_cfg;

uint8_t whoamI, rst;
static uint8_t tx_buffer[1000];

/*****************************************************************************/
/*  Interrupt Handlers                                                       */
/*****************************************************************************/
/* Interrupt handler callback function */
void Timer_Handler(void *handler_arg, cyhal_gpio_event_t event)
{
    static uint16_t count = 0;
    // check if it has been a minute 
    count++;
    if(count >= 600){
        count = 0;
        // 100ms count, notify game that the timer is up
        BaseType_t xHigherPriorityTaskWoken;

        /* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
        it will get set to pdTRUE inside the interrupt safe API function if a
        context switch is required. */
        xHigherPriorityTaskWoken = pdFALSE;

        /* ADD CODE */
        /* Send a notification directly to the task to which interrupt processing
        * is being deferred.
        */
        vTaskNotifyGiveFromISR(Task_End, &xHigherPriorityTaskWoken);

        /* ADD CODE */
        /* Call the function that will force the task that was running prior to the
        * interrupt to yield*/
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
/*****************************************************************************/
/*  HW03 Functions                                                           */
/*****************************************************************************/

/**
 * @brief
 * Initializes the PSoC6 Peripherals used for HW01
 */
void hw03_peripheral_init(void)
{
    /* Initialize the pushbuttons */
    push_buttons_init();

    /* Initialize the LCD */
    ece353_enable_lcd();

    /* Initialize the joystick*/
    joystick_init();

    /* Initialize the remote UART */
    remote_uart_init();
    // Enable interrupts and circular buffers from remote board.
    remote_uart_enable_interrupts();
    // init i2c for GPIO expander
    i2c_init();
    io_expander_set_configuration(0x80); 	// Set bit 7 as input, bits 6-0 as outputs 
	io_expander_set_output_port(0x00); 		// Turn OFF all LEDs

    // Init SPI
    spi_init();
    //initalize light sensor
    LTR_329_init();
    /* Initialize the EEPROM CS line*/
    eeprom_cs_init();
}

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
  cy_rslt_t rslt;
  uint8_t *tx;
  uint8_t *rx;

  /* Allocate memory for the spi Tx message */
  tx = malloc(sizeof(uint8_t) * (len + 1));

  /* Allocate memory for the spi Rx message */
  rx = malloc(sizeof(uint8_t) * (len + 1));

  // Write the register address to the first memory location in the Tx buffer
  tx[0] = reg & 0x7F;

  // Copy the remaining bytes to the Tx message
  memcpy(&tx[1], bufp, len);

  // Set the CS Low
  cyhal_gpio_write(PIN_SPI_IMU_CS, 0);

  // Starts a data transfer
  rslt = cyhal_spi_transfer(
    &mSPI,
    tx,
    len + 1,
    rx,
    len + 1,
    0x00
  );

  // Set the CS High
  cyhal_gpio_write(PIN_SPI_IMU_CS, 1);

  free(tx);
  free(rx);

  return (uint32_t)rslt;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
  cy_rslt_t rslt;
  uint8_t *tx;
  uint8_t *rx;

  /* Allocate memory for the spi Tx message */
  tx = malloc(sizeof(uint8_t) * len + 1);

  /* Allocate memory for the spi Rx message */
  rx = malloc(sizeof(uint8_t) * len + 1);

  // Write the register address to the first memory location in the Tx Message
  tx[0] = reg | 0x80;

  // set the remaining bytes in tx to 0
  memcpy(&tx[1], 0, len);

  // Set the CS Low
  cyhal_gpio_write(PIN_SPI_IMU_CS, 0);

  // Starts a data transfer
  rslt = cyhal_spi_transfer(
    &mSPI,
    tx,
    len + 1,
    rx,
    len + 1,
    0x00
  );

  // Set the CS High
  cyhal_gpio_write(PIN_SPI_IMU_CS, 1);

  // Copy the data returned from IMU to the destination address
  memcpy(bufp, &rx[1], len);

  free(tx);
  free(rx);

  return (uint32_t)rslt;
}

/*
 * @brief  Send buffer to console (platform dependent)
 *
 * @param  tx_buffer     buffer to transmit
 * @param  len           number of byte to send
 *
 */
void tx_com(uint8_t *tx_buffer, uint16_t len)
{
  printf("%s", tx_buffer);
}

/*
 * @brief  platform specific delay (platform dependent)
 *
 * @param  ms        delay in ms
 *
 */
void platform_delay(uint32_t ms)
{
  Cy_SysLib_Delay(ms);
}

/*
 * @brief  platform specific initialization (platform dependent)
 */
void platform_init(void)
{
  cyhal_gpio_init(
      PIN_SPI_IMU_CS,          // Pin
      CYHAL_GPIO_DIR_OUTPUT,   // Direction
      CYHAL_GPIO_DRIVE_STRONG, // Drive Mode
      true);                   // InitialValue
}

/**
 * @brief draws wait for player image
 * 
 */
void lcd_wait_for_other_player(void)
{
    lcd_draw_image(
        SCREEN_X / 3  + 5,
        180,
        WaitForOtherPlayerWidthPixels,
        WaitForOtherPlayerHeightPixels,
        WaitForOtherPlayerBitmaps,
        LCD_COLOR_GRAY,
        LCD_COLOR_GREEN);
}

/**
 * @brief Draws the tank in the vert directions
 * 
 * @param x position for tank
 * @param y position for tank
 * @param tankcolor for tank
 * @param colortread1 for tank
 * @param colortread2 for tank
 * @param background for map
 * @param one for tank tread logic
 */
void draw_Tank_BottomVert(uint16_t x, uint16_t y, int tankcolor, int colortread1, int colortread2, int background,  bool one){
    // Turn LCD On
    if(one){
        // draw background boarder
        lcd_draw_rectangle_centered(x, y, 34, 36, background);
        // draw body
        lcd_draw_image(
        x,
        y,
        tankBottomWidthPixels,
        tankBottomHeightPixels,
        tankBottomBitmaps,
        tankcolor,
        LCD_COLOR_BLACK);
        // draw one tread
        lcd_draw_image(
            x + 14,
            y,
            tankTreadWidthPixelsV,
            tankTreadHeightPixelsV,
            tankTread1BitmapsV,
            colortread1,
            colortread2);
        // draw other tread
        lcd_draw_image(
            x - 14,
            y,
            tankTreadWidthPixelsV,
            tankTreadHeightPixelsV,
            tankTread1BitmapsV,
            colortread1,
            colortread2);
    }else{
        // draw boarder for better movement
        lcd_draw_rectangle_centered(x, y, 34, 36, background);
        // draw body
        lcd_draw_image(
        x,
        y,
        tankBottomWidthPixels,
        tankBottomHeightPixels,
        tankBottomBitmaps,
        tankcolor,
        LCD_COLOR_BLACK);
        // draw right tread
        lcd_draw_image(
            x + 14,
            y,
            tankTreadWidthPixelsV,
            tankTreadHeightPixelsV,
            tankTread2BitmapsV,
            colortread1,
            colortread2);
        // draw left tread
        lcd_draw_image(
            x - 14,
            y,
            tankTreadWidthPixelsV,
            tankTreadHeightPixelsV,
            tankTread2BitmapsV,
            colortread1,
            colortread2);
        }
    
}

/**
 * @brief Draws the tank in the hori directions
 * 
 * @param x position for tank
 * @param y position for tank
 * @param tankcolor for tank
 * @param colortread1 for tank
 * @param colortread2 for tank
 * @param background for map
 * @param one for tank tread logic
 */
void draw_Tank_BottomHori(uint16_t x, uint16_t y, int tankcolor, int colortread1, int colortread2, int background, bool one){
    // Turn LCD On
    if(one){
        // draw boarder for better movement
        lcd_draw_rectangle_centered(x, y, 36, 34, background);
        // draw bottom
        lcd_draw_image(
        x,
        y,
        tankBottomHeightPixels,
        tankBottomWidthPixels,
        tankBottomBitmaps,
        tankcolor,
        LCD_COLOR_BLACK);

        // draw bottom tread
        lcd_draw_image(
            x,
            y + 14,
            tankTreadWidthPixelsH,
            tankTreadHeightPixelsH,
            tankTread1BitmapsH,
            colortread1,
            colortread2);
        
        // draw top tread
        lcd_draw_image(
            x,
            y - 14,
            tankTreadWidthPixelsH,
            tankTreadHeightPixelsH,
            tankTread1BitmapsH,
            colortread1,
            colortread2);
    }else{
        // draw boarder
        lcd_draw_rectangle_centered(x, y, 36, 36, background);
        // draw vertical bottom
        lcd_draw_image(
        x,
        y,
        tankBottomHeightPixels,
        tankBottomWidthPixels,
        tankBottomBitmaps,
        tankcolor,
        LCD_COLOR_BLACK);

        // draw bottom tread
        lcd_draw_image(
            x,
            y + 14,
            tankTreadWidthPixelsH,
            tankTreadHeightPixelsH,
            tankTread2BitmapsH,
            colortread1,
            colortread2);

        // draw top tread
        lcd_draw_image(
            x,
            y - 14,
            tankTreadWidthPixelsH,
            tankTreadHeightPixelsH,
            tankTread2BitmapsH,
            colortread1,
            colortread2);
    }
    // draw back for the fixing the 2 pixel movement
}

void draw_turr(Tank* tank){
    switch (tank->turr_dir)
    {
    case JOYSTICK_POS_UP:
        lcd_draw_image(
            tank->x,
            tank->y,
            tankTurrentWidthPixels,
            tankTurrentHeightPixels,
            tankTurrentDownBitmap,
            LCD_COLOR_YELLOW,
            tank->color);
        break;
    case JOYSTICK_POS_DOWN:
        lcd_draw_image(
            tank->x,
            tank->y,
            tankTurrentWidthPixels,
            tankTurrentHeightPixels,
            tankTurrentUpBitmap,
            LCD_COLOR_YELLOW,
            tank->color);
        break;
    case JOYSTICK_POS_RIGHT:
        lcd_draw_image(
            tank->x,
            tank->y,
            tankTurrentWidthPixels,
            tankTurrentHeightPixels,
            letterTBitmapRight,
            LCD_COLOR_YELLOW,
            tank->color);
        break;
    case JOYSTICK_POS_LEFT:
        lcd_draw_image(
            tank->x,
            tank->y,
            tankTurrentWidthPixels,
            tankTurrentHeightPixels,
            letterTBitmapLeft,
            LCD_COLOR_YELLOW,
            tank->color);
        break;
    case JOYSTICK_POS_UPPER_LEFT:
        lcd_draw_image(
            tank->x,
            tank->y,
            tankTurrentWidthPixels,
            tankTurrentHeightPixels,
            tankTurrenttopleftBitmap,
            LCD_COLOR_YELLOW,
            tank->color);
        break;
    case JOYSTICK_POS_UPPER_RIGHT:
        lcd_draw_image(
            tank->x,
            tank->y,
            tankTurrentWidthPixels,
            tankTurrentHeightPixels,
            tankTurrenttoprightBitmap,
            LCD_COLOR_YELLOW,
            tank->color);
        break;
    case JOYSTICK_POS_LOWER_LEFT:
        lcd_draw_image(
            tank->x,
            tank->y,
            tankTurrentWidthPixels,
            tankTurrentHeightPixels,
            tankTurrentlowerleftBitmap,
            LCD_COLOR_YELLOW,
            tank->color);
        break;
    case JOYSTICK_POS_LOWER_RIGHT:
        lcd_draw_image(
            tank->x,
            tank->y,
            tankTurrentWidthPixels,
            tankTurrentHeightPixels,
            tankTurrentlowerrightBitmap,
            LCD_COLOR_YELLOW,
            tank->color);
        break;
    }
}

/**
 * @brief moves a tank in direction
 * 
 * @param tank to move
 * @param direction to move
 */
void move_turr(Tank* tank, joystick_position_t direction){
    tank->turr_dir = direction;
    draw_turr(tank);
}

/**
 * @brief draws a tank
 * 
 * @param tank to draw
 * @param background color to draw background
 */
void draw_tank(Tank* tank, int background){
    switch(tank->tank_dir){
        case HORIZONTAL:
            draw_Tank_BottomHori(tank->x, tank->y, tank->color, LCD_COLOR_GRAY, LCD_COLOR_BLACK, background, tank->tread);
            break;
        case VERTICAL:
            draw_Tank_BottomVert(tank->x, tank->y, tank->color, LCD_COLOR_GRAY, LCD_COLOR_BLACK, background, tank->tread);
            break;
    }
    draw_turr(tank);
}

void move_tank_in_Tile(Tank* tank, Grid_Tile* battlefield, tank_direction direction, int8_t distance, int background){
    /* check if move is valid */
    // find current grid
    int16_t row = tank->y/40;
    int16_t col = tank->x/40;
    Grid_Tile * curr_tile = &battlefield[row * 8 + col];
    Grid_Tile * dest_tile;
    // find next grid if in it
    switch (direction)
    {
        case HORIZONTAL:
            col = (tank->x + 17*distance)/40;
            dest_tile = &battlefield[row * 8 + col];
            break;
        case VERTICAL:
            row = (tank->y + 17*distance)/40;
            dest_tile = &battlefield[row * 8 + col];
            break;
    }

    /*if grid is occupied dont move */ 
    if(dest_tile->occupied && dest_tile->claimed != tank->ID ){
        return; // another tank is in there
    }

    /* if at center and moving towards direction of the wall, don't move */
    if(direction == HORIZONTAL){
        if(tank->x == curr_tile->x){
            // check wall right
            if(distance > 0){
                if(curr_tile->right){
                    return;
                }
            }
            // check wall left
            else if(distance < 0){
                if(curr_tile->left){
                    return;
                }
            }
        }
        else{
            if(dest_tile != curr_tile){
                int16_t half = curr_tile->y - tank->y;
                // check if in bottom half of grid
                // check if in it might over right a wall
                if((dest_tile->top || dest_tile->bottom) && (half > 3 || half < -3)){
                    return;
                }
            }
        }
    }else{
        if(tank->y == curr_tile->y){
            // check wall above
            if(distance < 0 && curr_tile->top){
                return;
            }
            // check wall below
            else if (distance > 0 && curr_tile->bottom){
                return;
            }
        }else{
            if(dest_tile != curr_tile){
                int16_t half = curr_tile->x - tank->x;
                // check if in it might over right a wall
                if((dest_tile->right || dest_tile->left) && (half > 3 || half < -3)){
                    return;
                }
            }
        }
    }
    
    /* Move Tank */
    switch (direction)
    {
        case HORIZONTAL:
            tank->x += distance;
            break;
        case VERTICAL:
            tank->y += distance;
            break;
    }
    // if leaving grid update new grid that it's occupied
    if(curr_tile != dest_tile){
        dest_tile->occupied = true;
        dest_tile->claimed = tank->ID;
        curr_tile->occupied = false;
    }
    // check if leaving old grid and make it free
    tank->tank_dir = direction;
    draw_tank(tank, background);
    tank->tread = !tank->tread;
}

void move_tank(Tank* tank, tank_direction direction, int8_t distance, int background){
    switch (direction)
    {
    case HORIZONTAL:
        tank->x += distance;
        break;
    case VERTICAL:
        tank->y += distance;
        break;
    }
    tank->tank_dir = direction;
    draw_tank(tank, background);
    tank->tread = !tank->tread;
}

void init_battlefield(Grid_Tile* battlefield){
    // set all centers to the x, y pixel location
    Grid_Tile *curr_grid;
    for (uint16_t i = 0; i < 6; i++) {
        for (uint16_t j = 0; j < 8; j++){
            curr_grid = &battlefield[i * 8 + j];
            curr_grid->x = j*40 + 20;
            curr_grid->y = i*40 + 20;
            curr_grid->occupied = false;
        }
    }
    // manually set all grids to draw map
    // grid 1 j is col, i is row
    curr_grid = &battlefield[0 * 8 + 0];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = false;
    curr_grid->left = true;
    // grid 2 j is col, i is row
    curr_grid = &battlefield[0 * 8 + 1];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 3 j is col, i is row
    curr_grid = &battlefield[0 * 8 + 2];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 4 j is col, i is row
    curr_grid = &battlefield[0 * 8 + 3];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 5 j is col, i is row
    curr_grid = &battlefield[0 * 8 + 4];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = true;
    curr_grid->left = false;
    // grid 6 j is col, i is row
    curr_grid = &battlefield[0 * 8 + 5];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = false;
    curr_grid->left = true;
    // grid 7 j is col, i is row
    curr_grid = &battlefield[0 * 8 + 6];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 8 j is col, i is row
    curr_grid = &battlefield[0 * 8 + 7];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = true;
    curr_grid->left = false;

    // row 2
    // grid 1 j is col, i is row
    curr_grid = &battlefield[1 * 8 + 0];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = true;
    // grid 2 j is col, i is row
    curr_grid = &battlefield[1 * 8 + 1];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 3 j is col, i is row
    curr_grid = &battlefield[1 * 8 + 2];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 4 j is col, i is row
    curr_grid = &battlefield[1 * 8 + 3];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 5 j is col, i is row
    curr_grid = &battlefield[1 * 8 + 4];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 6 j is col, i is row
    curr_grid = &battlefield[1 * 8 + 5];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 7 j is col, i is row
    curr_grid = &battlefield[1 * 8 + 6];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = true;
    curr_grid->left = false;
    // grid 8 j is col, i is row
    curr_grid = &battlefield[1 * 8 + 7];
    curr_grid->top = false;
    curr_grid->bottom = false;
    curr_grid->right = true;
    curr_grid->left = true;

    // row 3
    // grid 1 j is col, i is row
    curr_grid = &battlefield[2 * 8 + 0];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = true;
    curr_grid->left = true;
    // grid 2 j is col, i is row
    curr_grid = &battlefield[2 * 8 + 1];
    curr_grid->top = false;
    curr_grid->bottom = false;
    curr_grid->right = true;
    curr_grid->left = true;
    // grid 3 j is col, i is row
    curr_grid = &battlefield[2 * 8 + 2];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = false;
    curr_grid->left = true;
    // grid 4 j is col, i is row
    curr_grid = &battlefield[2 * 8 + 3];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 5 j is col, i is row
    curr_grid = &battlefield[2 * 8 + 4];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 6 j is col, i is row
    curr_grid = &battlefield[2 * 8 + 5];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = true;
    curr_grid->left = false;
    // grid 7 j is col, i is row
    curr_grid = &battlefield[2 * 8 + 6];
    curr_grid->top = false;
    curr_grid->bottom = false;
    curr_grid->right = false;
    curr_grid->left = true;
    // grid 8 j is col, i is row
    curr_grid = &battlefield[2 * 8 + 7];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = true;
    curr_grid->left = false;

    // row 4
    // grid 1 j is col, i is row
    curr_grid = &battlefield[3 * 8 + 0];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = false;
    curr_grid->left = true;
    // grid 2 j is col, i is row
    curr_grid = &battlefield[3 * 8 + 1];
    curr_grid->top = false;
    curr_grid->bottom = false;
    curr_grid->right = true;
    curr_grid->left = false;
    // grid 3 j is col, i is row
    curr_grid = &battlefield[3 * 8 + 2];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = true;
    // grid 4 j is col, i is row
    curr_grid = &battlefield[3 * 8 + 3];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 5 j is col, i is row
    curr_grid = &battlefield[3 * 8 + 4];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 6 j is col, i is row
    curr_grid = &battlefield[3 * 8 + 5];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = true;
    curr_grid->left = false;
    // grid 7 j is col, i is row
    curr_grid = &battlefield[3 * 8 + 6];
    curr_grid->top = false;
    curr_grid->bottom = false;
    curr_grid->right = true;
    curr_grid->left = true;
    // grid 8 j is col, i is row
    curr_grid = &battlefield[3 * 8 + 7];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = true;
    curr_grid->left = true;

    // row 5
    // grid 1 j is col, i is row
    curr_grid = &battlefield[4 * 8 + 0];
    curr_grid->top = false;
    curr_grid->bottom = false;
    curr_grid->right = true;
    curr_grid->left = true;
    // grid 2 j is col, i is row
    curr_grid = &battlefield[4 * 8 + 1];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = true;
    // grid 3 j is col, i is row
    curr_grid = &battlefield[4 * 8 + 2];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 4 j is col, i is row
    curr_grid = &battlefield[4 * 8 + 3];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 5 j is col, i is row
    curr_grid = &battlefield[4 * 8 + 4];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 6 j is col, i is row
    curr_grid = &battlefield[4 * 8 + 5];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 7 j is col, i is row
    curr_grid = &battlefield[4 * 8 + 6];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 8 j is col, i is row
    curr_grid = &battlefield[4 * 8 + 7];
    curr_grid->top = true;
    curr_grid->bottom = false;
    curr_grid->right = true;
    curr_grid->left = false;

    // row 6
    // grid 1 j is col, i is row
    curr_grid = &battlefield[5 * 8 + 0];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = true;
    // grid 2 j is col, i is row
    curr_grid = &battlefield[5 * 8 + 1];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 3 j is col, i is row
    curr_grid = &battlefield[5 * 8 + 2];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = true;
    curr_grid->left = false;
    // grid 4 j is col, i is row
    curr_grid = &battlefield[5 * 8 + 3];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = true;
    // grid 5 j is col, i is row
    curr_grid = &battlefield[5 * 8 + 4];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 6 j is col, i is row
    curr_grid = &battlefield[5 * 8 + 5];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 7 j is col, i is row
    curr_grid = &battlefield[5 * 8 + 6];
    curr_grid->top = true;
    curr_grid->bottom = true;
    curr_grid->right = false;
    curr_grid->left = false;
    // grid 8 j is col, i is row
    curr_grid = &battlefield[5 * 8 + 7];
    curr_grid->top = false;
    curr_grid->bottom = true;
    curr_grid->right = true;
    curr_grid->left = false;

    
}

/**
 * @brief
 * receives tank coord and orientation and creates a dynamically allocated bullet
 * @param tank
 * tank to generate bullet from
 * @param bullet
 * bullet to pass values to
 * NOTE: may have to get tank id, if other player's tank, invert coords and directions
 */
void shoot_bullet(Tank* tank, Bullet* bullet){

    bullet->damage = 1;
    bullet->color = LCD_COLOR_WHITE;
    bullet->num_bounce = 1; //0 = no bullet, 4 = 3rd bounce, set to 0
    /*check orientation of tank turr, and give bullet respective coordinates*/
    switch (tank->turr_dir)
    {
        case JOYSTICK_POS_UP:
        bullet->x = tank->x;
        bullet->y = tank->y - 15;
        bullet->bullet_dir = UP;
        break;

        case JOYSTICK_POS_DOWN:
        bullet->x = tank->x;
        bullet->y = tank->y + 15;
        bullet->bullet_dir = DOWN;
        break;

        case JOYSTICK_POS_LEFT:
        bullet->x = tank->x - 15;
        bullet->y = tank->y;
        bullet->bullet_dir = LEFT;
        break;

        case JOYSTICK_POS_RIGHT:
        bullet->x = tank->x + 15;
        bullet->y = tank->y;
        bullet->bullet_dir = RIGHT;
        break;

        case JOYSTICK_POS_UPPER_RIGHT:
        bullet->x = tank->x + 15;
        bullet->y = tank->y - 15;
        bullet->bullet_dir = UP_RIGHT;
        break;

        case JOYSTICK_POS_LOWER_RIGHT:
        bullet->x = tank->x + 15;
        bullet->y = tank->y + 15;
        bullet->bullet_dir = DOWN_RIGHT;
        break;

        case JOYSTICK_POS_UPPER_LEFT:
        bullet->x = tank->x - 15;
        bullet->y = tank->y - 15;
        bullet->bullet_dir = UP_LEFT;
        break;

        case JOYSTICK_POS_LOWER_LEFT:
        bullet->x = tank->x - 15;
        bullet->y = tank->y + 15;
        bullet->bullet_dir = DOWN_LEFT;
        break;
    }

}

/**
 * @brief
 * draws bullet on lcd screen
 * @param bullet
 * bullet to draw
 */
void draw_bullet(Bullet* bullet, uint16_t color){
    lcd_draw_rectangle_centered(bullet->x, bullet->y, BULLET_SIZE, BULLET_SIZE, color);
}

/**
 * @brief
 * check boundaries for bullet, bounce off wall boundaries
 * @param bullet
 * bullet to draw
 * @param battlefield
 * field to check bounds against
 */
void bounce_bullet(Bullet* bullet, Grid_Tile* battlefield){
    int row = bullet->y/40;
    int col = bullet->x/40;
    //find what grid bullet is in
    Grid_Tile *curr_grid = &battlefield[row * 8 + col]; 
    //check what walls are in grid
    //for each wall, check if near coordinates for that wall.
    //if so, invert the respective bullet direction

    /*right wall*/
    if(curr_grid->right){ //only need to check right bullet directions
        if(bullet->x > (curr_grid->x + 15)){//bullet hits wall
            if(bullet->bullet_dir == RIGHT){
                bullet->bullet_dir = LEFT;
                bullet->num_bounce +=1;
            }
            if(bullet->bullet_dir == UP_RIGHT){
                bullet->bullet_dir = UP_LEFT;
                bullet->num_bounce +=1;
            }
            if(bullet->bullet_dir == DOWN_RIGHT){
                bullet->bullet_dir = DOWN_LEFT;
                bullet->num_bounce +=1;
            }
        }
    }
    /*left wall*/
    if(curr_grid->left){ //only need to check left bullet directions
        if(bullet->x < (curr_grid->x - 15)){//bullet hits wall
            if(bullet->bullet_dir == LEFT){
                bullet->bullet_dir = RIGHT;
                bullet->num_bounce +=1;
            }
            if(bullet->bullet_dir == UP_LEFT){
                bullet->bullet_dir = UP_RIGHT;
                bullet->num_bounce +=1;
            }
            if(bullet->bullet_dir == DOWN_LEFT){
                bullet->bullet_dir = DOWN_RIGHT;
                bullet->num_bounce +=1;
            }
        }
    }
    /*upper wall*/
    if(curr_grid->top){ //only need to check up bullet directions
        if(bullet->y < (curr_grid->y - 15)){//bullet hits wall
            if(bullet->bullet_dir == UP){
                bullet->bullet_dir = DOWN;
                bullet->num_bounce +=1;
            }
            if(bullet->bullet_dir == UP_LEFT){
                bullet->bullet_dir = DOWN_LEFT;
                bullet->num_bounce +=1;
            }
            if(bullet->bullet_dir == UP_RIGHT){
                bullet->bullet_dir = DOWN_RIGHT;
                bullet->num_bounce +=1;
            }
        }
    }
    /*lower wall*/
    if(curr_grid->bottom){ //only need to check down bullet directions
        if(bullet->y > (curr_grid->y + 15)){//bullet hits wall
            if(bullet->bullet_dir == DOWN){
                bullet->bullet_dir = UP;
                bullet->num_bounce +=1;
            }
            if(bullet->bullet_dir == DOWN_LEFT){
                bullet->bullet_dir = UP_LEFT;
                bullet->num_bounce +=1;
            }
            if(bullet->bullet_dir == DOWN_RIGHT){
                bullet->bullet_dir = UP_RIGHT;
                bullet->num_bounce +=1;
            }
        }
    }

}

/**
 * @brief
 * moves bullet and calls bounds checking and hit reg
 * @param bullet
 * bullet to move
 * @param battlefield
 * field to check bounds against
 * @param bgcolor
 * color to replace old bullet with
 */
void move_bullet(Bullet* bullet, Grid_Tile* battlefield, uint16_t bgcolor){
    //replace old bullet with background
    draw_bullet(bullet, bgcolor);

    //update coordinates of bullet
    switch (bullet->bullet_dir)
    {
        case UP:
        bullet->y -= 3;
        break;

        case DOWN:
        bullet->y += 3;
        break;

        case LEFT:
        bullet->x -= 3;
        break;

        case RIGHT:
        bullet->x += 3;
        break;

        case UP_RIGHT:
        bullet->x += 3;
        bullet->y -= 3;
        break;

        case DOWN_RIGHT:
        bullet->x += 3;
        bullet->y += 3;
        break;

        case UP_LEFT:
        bullet->x -= 3;
        bullet->y -= 3;
        break;

        case DOWN_LEFT:
        bullet->x -= 3;
        bullet->y += 3;
        break;
    }
    /**
     * TODO: 
     * bounce checking 
     * hit reg checking
     */
    bounce_bullet(bullet, battlefield);
    //if num_bounce is greater than 4, set to 0 and "remove" bullet by hiding it
    if(bullet->num_bounce > 4){
        bullet->num_bounce = 0;
        bullet->damage = 0; //prevent ghost bullets from hurting players
        draw_bullet(bullet, bgcolor); //remove bullet
    }else{
    //draw new bullet
    draw_bullet(bullet, bullet->color);
    }
}
/**
 * @brief
 * obtains area of tank and compares if bullet is within that area
 * @param bullet
 * bullet to be checked
 * @param tank
 * tank to check bounds against
 */
bool bullet_hit(Bullet* bullet, Tank* tank, u_int16_t bgcolor){

    if((abs(bullet->x - tank->x) < 16) && (abs(bullet->y - tank->y) < 16)){
        //a hit is registered, remove bullet
        bullet->num_bounce = 0;
        bullet->damage = 0; //prevent ghost bullets from hurting players
        draw_bullet(bullet, bgcolor); //remove bullet
        return true;
    }

    return false;

}

/**
 * @brief
 * obtains battle field array and draws the battlefield
 * @param battlefield
 * battle field array
 * @param wallcolor
 * color to draw walls
 */
void draw_battlefield(Grid_Tile* battlefield, int wallcolor){
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 8; j++){
            Grid_Tile *curr_grid = &battlefield[i * 8 + j];
            // draw top
            if(curr_grid->top){
                lcd_draw_rectangle(curr_grid->x-20, 40,curr_grid->y-20, 3, wallcolor);
            }
            // draw bottom
            if(curr_grid->bottom){
                lcd_draw_rectangle(curr_grid->x-20, 40,curr_grid->y+17, 3, wallcolor);
            }
            // draw right
            if(curr_grid->right){
                lcd_draw_rectangle(curr_grid->x+17, 3,curr_grid->y-20, 40, wallcolor);
            }
            // draw left
            if(curr_grid->left){
                lcd_draw_rectangle(curr_grid->x-20, 3,curr_grid->y-20, 40, wallcolor);
            }
        }
    }
}

/**
 * @brief
 * Implements the main application for HW03
 */
void hw03_main_app(void)
{
    /* RTOS TASK SETUP */
    /* Semiphores */
    Sem_SW1 = xSemaphoreCreateBinary();
    Sem_SW2 = xSemaphoreCreateBinary();

    /* QUEUEs */
    Queue_Joystick_Position = xQueueCreate(1, sizeof(joystick_position_t));
    Queue_Light = xQueueCreate(1, sizeof(uint32_t));
    Queue_Player_Win = xQueueCreate(1, sizeof(uint8_t));
    Queue_Player_Choice = xQueueCreate(1, sizeof(uint8_t));
    Queue_Gyro_X_Position = xQueueCreate(1, sizeof(uint8_t));
    Queue_Gyro_Y_Position = xQueueCreate(1, sizeof(uint8_t));
    Queue_RX = xQueueCreate(1, sizeof(uint8_t));
    Queue_TX = xQueueCreate(1, sizeof(uint8_t));

    /* TASKs */
    /* Create SW1 Task */
    xTaskCreate(
        task_button_sw1, // impliments task
        "SW1 Pressed Task", // idenifier
        configMINIMAL_STACK_SIZE, // stack size
        NULL, // passed in parameters
        2, // priority (1 is lowest (idle), higher is more important )
        NULL // task handle for notifications
    );
    /* Create SW2 Task */
    xTaskCreate(
        task_button_sw2, // impliments task
        "SW2 Pressed Task", // idenifier
        configMINIMAL_STACK_SIZE, // stack size
        NULL, // passed in parameters
        2, // priority (1 is lowest (idle), higher is more important )
        NULL // task handle for notifications
    );
    /* Create Joystick Task */
    xTaskCreate(
        task_joystick, // impliments task
        "Joystick Task", // idenifier
        configMINIMAL_STACK_SIZE, // stack size
        NULL, // passed in parameters
        2, // priority (1 is lowest (idle), higher is more important )
        NULL // task handle for notifications
    );
    /* Create start Task */
    xTaskCreate(
        task_start, // impliments task
        "Start Screen Task", // idenifier
        configMINIMAL_STACK_SIZE, // stack size
        NULL, // passed in parameters
        3, // priority (1 is lowest (idle), higher is more important )
        NULL // task handle for notifications
    );

    /* Create End Task */
    xTaskCreate(
        task_end, // impliments task
        "End Screen Task", // idenifier
        configMINIMAL_STACK_SIZE, // stack size
        NULL, // passed in parameters
        3, // priority (1 is lowest (idle), higher is more important )
        &Task_End // task handle for notifications
    );
    /* Create Save Eeprom Task */
    xTaskCreate(
        task_save_eeprom, // impliments task
        "Save EEPROM Task", // idenifier
        configMINIMAL_STACK_SIZE, // stack size
        NULL, // passed in parameters
        2, // priority (1 is lowest (idle), higher is more important )
        &Task_Save_Eeprom // task handle for notifications
    );
    /*create UART_TX*/
    xTaskCreate(
        task_uart_tx, // impliments task
        "UART send task", // idenifier
        configMINIMAL_STACK_SIZE, // stack size
        NULL, // passed in parameters
        2, // priority (1 is lowest (idle), higher is more important )
        &Task_UART_Tx // task handle for notifications
    );
    xTaskCreate(
        task_uart_rx, // impliments task
        "Uart Receive task", // idenifier
        configMINIMAL_STACK_SIZE, // stack size
        NULL, // passed in parameters
        2, // priority (1 is lowest (idle), higher is more important )
        &Task_UART_Rx // task handle for notifications
    );
    xTaskCreate(
        task_gyro, // impliments task
        "receive gyro data", // idenifier
        configMINIMAL_STACK_SIZE, // stack size
        NULL, // passed in parameters
        2, // priority (1 is lowest (idle), higher is more important )
        NULL// task handle for notifications
    );

    /* Start the Scheduler */
    vTaskStartScheduler();
    while(1)
    {  
         CY_ASSERT(0);
    }
}