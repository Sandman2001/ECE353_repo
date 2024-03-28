/*
 * opt3001.h
 *
 *  Created on: Oct 20, 2020
 *      Author: Joe Krachey
 */

#ifndef IO_EXPANDER_H_
#define IO_EXPANDER_H_

#include <stdint.h>
#include <stdbool.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#include "i2c.h"

/* ADD CODE */
#define TCA9534_SUBORDINATE_ADDR                 0x20 // last bit is read/write

/* ADD CODE for IO Expander Register Addresses */
#define TCA9534_INPUT_PORT_ADDR					 0x00
#define TCA9534_OUTPUT_PORT_ADDR				 0x01
#define TCA9534_POLARITY_ADDR					 0x02
#define TCA9534_CONFIG_ADDR						 0x03

extern bool ALERT_IO_EXPANDER;

/** Read the value of the input port
 *
 * @param reg The reg address to read
 *
 */
uint8_t io_expander_get_input_port(void);

/** Write the value of the output port
 *
 * @param value The value written
 *
 */
void io_expander_set_output_port(uint8_t value);

/** Write the polarity inversion register
 *
 * @param value The value written
 *
 */
void io_expander_set_polarity_inversion(uint8_t value);

/** Write the value of the configuration Register
 *
 * @param value The value written
 *
 */
void io_expander_set_configuration(uint8_t value);

#endif /* IO_EXPANDER_H_ */
