/**
 * @file hw03.h
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-09-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __HW03_H__
#define __HW03_H__

#include "drivers/joystick.h"

#define SCREEN_X            320
#define SCREEN_Y            240

#define SCREEN_CENTER_COL    ((SCREEN_X/2)-1)
#define SCREEN_CENTER_ROW    ((SCREEN_Y/2)-1)

#define LINE_WIDTH          4
#define LINE_LENGTH         120 

#define SQUARE_SIZE         32

#define PADDING             2

#define BULLET_SIZE         5

#define LEFT_COL                      (SCREEN_CENTER_COL - SQUARE_SIZE - (2*PADDING) - LINE_WIDTH)
#define CENTER_COL                    (SCREEN_CENTER_COL)
#define RIGHT_COL                     (SCREEN_CENTER_COL + SQUARE_SIZE + (2*PADDING) + LINE_WIDTH)

#define UPPER_ROW                     (SCREEN_CENTER_ROW - SQUARE_SIZE - (2*PADDING) - LINE_WIDTH + 20 )
#define CENTER_ROW                    (SCREEN_CENTER_ROW + 20 )
#define LOWER_ROW                     (SCREEN_CENTER_ROW + SQUARE_SIZE + (2*PADDING) + LINE_WIDTH + 20 )

#define UPPER_HORIZONTAL_LINE_Y     (SCREEN_CENTER_ROW - (SQUARE_SIZE/2) - PADDING - LINE_WIDTH/2 + 20)
#define LOWER_HORIZONTAL_LINE_Y     (SCREEN_CENTER_ROW + (SQUARE_SIZE/2) + PADDING + LINE_WIDTH/2 + 20)

#define LEFT_VERTICAL_LINE_X      (SCREEN_CENTER_COL - (SQUARE_SIZE/2) - PADDING - LINE_WIDTH/2)
#define RIGHT_VERTICAL_LINE_X     (SCREEN_CENTER_COL + (SQUARE_SIZE/2) + PADDING + LINE_WIDTH/2)

#define LEFT_HORIZONTAL_LINE_X      (SCREEN_CENTER_COL - (SQUARE_SIZE/2) - PADDING - LINE_WIDTH/2)
#define RIGHT_HORIZONTAL_LINE_X     (SCREEN_CENTER_COL + (SQUARE_SIZE/2) + PADDING + LINE_WIDTH/2)

#define FG_COLOR_X                LCD_COLOR_YELLOW
#define BG_COLOR_X                LCD_COLOR_BLACK
#define FG_COLOR_O                LCD_COLOR_CYAN
#define BG_COLOR_O                LCD_COLOR_BLACK

#define FG_COLOR_CLAIMED          LCD_COLOR_BLACK
#define BG_COLOR_CLAIMED          LCD_COLOR_RED

#define FG_COLOR_UNCLAIMED        LCD_COLOR_BLACK
#define BG_COLOR_UNCLAIMED        LCD_COLOR_GREEN

#define MESSAGE_PLAYER1_SELECTION 0x5A
#define MESSAGE_ACK               0xF0
#define MESSAGE_NACK              0xE0
#define MESSAGE_X                 0x58
#define MESSAGE_O                 0x4F
#define MESSAGE_RESTART           0x30
#define MESSAGE_INVALID           0x90

typedef enum
{
    EVENT_NONE,
    EVENT_100MS,
    EVENT_SW1,
    EVENT_SW2,
    EVENT_JOYSTICK
} events_t;

typedef enum
{
    VERTICAL,
    HORIZONTAL  
} tank_direction;

typedef enum
{
    UP,
    DOWN,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT,
    LEFT,
    RIGHT
} bullet_direction;

typedef enum
{
    START,
    PLAY,
    END,
    WAIT
}GAME_STATE;

typedef struct {
    uint16_t x;          // X position of the tank
    uint16_t y;          // Y position of the tank
    uint8_t health;     // Health of the tank
    bool tread;         // tread animation
    uint16_t color;    // color of tank body
    joystick_position_t turr_dir; // direction to draw turr
    tank_direction tank_dir; // direction to draw tank body
    int8_t ID;
    uint8_t spawn_row;
    uint8_t spawn_col;
} Tank;
typedef struct {
    bool top; // draw wall on top of grid
    bool bottom; // draw wall on bottom of grid
    bool right; // draw wall on right of grid
    bool left; // draw wall on left of grid
    uint16_t x;
    uint16_t y;
    bool occupied;
    int8_t claimed;
} Grid_Tile;
typedef struct{
    uint16_t x;          // X position of the bullet
    uint16_t y;          // Y position of the bullet
    uint8_t damage;      // amount of damage to deal to tank
    uint16_t color;    // color of bullet
    uint8_t num_bounce; // number of bounces occured for this bullet
    bullet_direction bullet_dir;
}Bullet;

extern SemaphoreHandle_t Sem_SW1;
extern SemaphoreHandle_t Sem_SW2;

extern QueueHandle_t Queue_Joystick_Position;
extern QueueHandle_t Queue_Light;
extern QueueHandle_t Queue_RX;
extern QueueHandle_t Queue_TX;
extern QueueHandle_t Queue_Player_Choice;
extern QueueHandle_t Queue_Player_Win;
extern QueueHandle_t Queue_Gyro_X_Position;
extern QueueHandle_t Queue_Gyro_Y_Position;

extern TaskHandle_t Task_Game;
extern TaskHandle_t Task_End;
extern TaskHandle_t Task_Save_Eeprom;
extern TaskHandle_t Task_UART_Rx;
extern TaskHandle_t Task_UART_Tx;

extern uint8_t whoamI, rst;
extern cyhal_timer_t hw03_timer_obj;
extern cyhal_timer_cfg_t hw03_timer_cfg;


extern char HW03_DESCRIPTION[];
extern volatile events_t HW03_ALERT_100MS;
extern volatile events_t HW03_ALERT_SW1;
extern volatile events_t HW03_ALERT_SW2;

/* Timer object used */
extern cyhal_timer_t hw03_timer_obj;
extern cyhal_timer_cfg_t hw03_timer_cfg;
extern int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len);
extern int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);
extern void tx_com(uint8_t *tx_buffer, uint16_t len);
extern void platform_delay(uint32_t ms);
extern void platform_init(void);
extern void lcd_wait_for_other_player(void);
extern void draw_Tank_BottomVert(uint16_t x, uint16_t y, int tankcolor, int colortread1, int colortread2, int background,  bool one);
extern void draw_Tank_BottomHori(uint16_t x, uint16_t y, int tankcolor, int colortread1, int colortread2, int background, bool one);
extern void draw_turr(Tank* tank);
extern void move_turr(Tank* tank, joystick_position_t direction);
extern void draw_tank(Tank* tank, int background);
extern void move_tank_in_Tile(Tank* tank, Grid_Tile* battlefield, tank_direction direction, int8_t distance, int background);
extern void move_tank(Tank* tank, tank_direction direction, int8_t distance, int background);
extern void init_battlefield(Grid_Tile* battlefield);
extern void shoot_bullet(Tank* tank, Bullet* bullet);
extern void draw_bullet(Bullet* bullet, uint16_t color);
extern void bounce_bullet(Bullet* bullet, Grid_Tile* battlefield);
extern void move_bullet(Bullet* bullet, Grid_Tile* battlefield, uint16_t bgcolor);
extern bool bullet_hit(Bullet* bullet, Tank* tank, u_int16_t bgcolor);
extern void draw_battlefield(Grid_Tile* battlefield, int wallcolor);

void Timer_Handler(void *handler_arg, cyhal_gpio_event_t event);

/**
 * @brief 
 * Initializes the PSoC6 Peripherals used for HW02 
 */
void hw03_peripheral_init(void);

/**
 * @brief 
 * Implements the main application for HW02 
 */
void hw03_main_app(void);

#endif