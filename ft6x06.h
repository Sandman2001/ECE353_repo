#ifndef __FT6X06_H__
#define __FT6X06_H__

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "stdio.h"

#include "i2c.h"

#define PIN_IO_CAP_TOUCH_INT           P11_1

#define FT6X06_I2C_ADDR                 0x38

//*****************************************************************************
// Fill out the #defines below to configure which pins are connected to
// the I2C Bus
//*****************************************************************************
#define FT6X06_DEV_MODE_R             0x00
#define FT6X06_GET_ID_R               0x01
#define FT6X06_TD_STATUS_R            0x02
#define FT6X06_P1_XH_R                0x03
#define FT6X06_P1_XL_R                0x04
#define FT6X06_P1_YH_R                0x05
#define FT6X06_P1_YL_R                0x06
#define FT6X06_P1_WEIGHT_R            0x07
#define FT6X06_P1_MISC_R              0x08
#define FT6X06_P2_XH_R                0x09
#define FT6X06_P2_XL_R                0x0A
#define FT6X06_P2_YH_R                0x0B
#define FT6X06_P2_YL_R                0x0C
#define FT6X06_P2_WEIGHT_R            0x0D
#define FT6X06_P2_MISC_R              0x0E
#define FT6X06_TH_GROUP_R             0x80
#define FT6X06_TH_DIFF_R              0x85
#define FT6X06_CTRL_R                 0x86
#define FT6X06_TIMEENTERMONITOR_R     0x87
#define FT6X06_PERIODACTIVITY_R       0x88
#define FT6X06_PERIODMONITOR_R        0x89
#define FT6X06_RADIAN_VALUE_R         0x91
#define FT6X06_OFFSET_LEFT_RIGHT_R    0x92
#define FT6X06_OFFSET_UP_DOWN_R       0x93
#define FT6X06_DISTANCE_LEFT_RIGHT_R  0x94
#define FT6X06_DISTANCE_UP_DOWN_R     0x95
#define FT6X06_DISTANCE_ZOOM_R        0x96
#define FT6X06_LIB_VER_H_R            0xA1
#define FT6X06_LIB_VER_L_R            0xA2
#define FT6X06_CIPHER_R               0xA3
#define FT6X06_G_MODE_R               0xA4
#define FT6X06_POWER_MODE_R           0xA5
#define FT6X06_FIRMID_R               0xA6
#define FT6X06_FOCALTECH_ID_R         0xA8
#define FT6X06_REALEASE_CODE_ID_R     0xAF
#define FT6X06_STATE_R                0xBC

//*****************************************************************************
// Test the ft6x06
//*****************************************************************************
void test_ft6x06(void);

//*****************************************************************************
// Initialize the ft6x06 Interrupt pin
//*****************************************************************************
cy_rslt_t ft6x06_init_gpio_interrupt(void);

//*****************************************************************************
// Read the number of active touch points.
//*****************************************************************************
uint8_t ft6x06_read_td_status(void);

//*****************************************************************************
// Read the X value of last touch event
//*****************************************************************************
uint16_t ft6x06_read_x(void);

//*****************************************************************************
// Read the Y value of last touch event
//*****************************************************************************
uint16_t ft6x06_read_y(void);

//*****************************************************************************
// Test the ft6x06
//*****************************************************************************
void test_ft6x06(void);

#endif

