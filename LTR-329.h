/*
 * opt3001.h
 *
 *  Created on: dec 2, 2023
 *      Author: Andrew Sanders
 */

#ifndef LTR_329_H_
#define LTR_329_H_

#include <stdint.h>
#include <stdbool.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#define LTR_329_SUBORDINATE_ADDR              0x29
#define LTR_329_CTRL_REG                      0x80
#define LTR_329_DEV_ID_REG                    0x86
#define LTR_329_DEV_STATUS_REG                0x8C
#define LTR_329_INIT_CMD                      0x01
#define LTR_329_LIGHT_REG_CH1				  0x88
#define LTR_329_LIGHT_REG_CH0				  0x8A

/**
 *
 *
 *
 */
cy_rslt_t LTR_329_init(void);

/** Read the temperature from LTR_329
 *
 * @param
 *
 */
int32_t LTR_329_get_light(void);


#endif /* LTR_329 */
