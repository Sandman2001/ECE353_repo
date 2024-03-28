/**
 * @file LTR-328.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-09-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "i2c.h"
#include "LTR-329.h"


cy_rslt_t LTR_329_init(void){
	cy_rslt_t rslt;

	uint8_t write_data[2] = {LTR_329_CTRL_REG, LTR_329_INIT_CMD}; //init sensor to start reading
	rslt = cyhal_i2c_master_write(&i2c_monarch_obj, LTR_329_SUBORDINATE_ADDR, write_data, 2, 0, true);

	CY_ASSERT(rslt == CY_RSLT_SUCCESS); /* If the write failed, halt the CPU */

}
/** Read a register on the LTR_329
 *
 * @param reg The reg address to read
 *
 */
static int32_t LTR_329_read_reg(uint8_t reg)
{
	int32_t return_value;
	cy_rslt_t rslt;

	uint8_t write_data[1] = {reg}; 
	uint8_t read_data[4]; //array of 4 to receive data
	uint8_t status[1];
    uint8_t get_status[1] = {LTR_329_DEV_STATUS_REG};

	/* Use cyhal_i2c_master_write to write the required data to the device. 
	 * Send the register address and then generate a stop condition 
	 */
	//check status register. will return h'4 if new data available
	rslt = cyhal_i2c_master_write(&i2c_monarch_obj, LTR_329_SUBORDINATE_ADDR, get_status, 1, 0, true);
	CY_ASSERT(rslt == CY_RSLT_SUCCESS); /* If the write failed, halt the CPU */

	rslt = cyhal_i2c_master_read(&i2c_monarch_obj, LTR_329_SUBORDINATE_ADDR, status, 1, 0, true);
		
	CY_ASSERT(rslt == CY_RSLT_SUCCESS); /* If the read failed, halt the CPU */

	if(status[0] & 0x4){ //if status bit has new data

		rslt = cyhal_i2c_master_write(&i2c_monarch_obj, LTR_329_SUBORDINATE_ADDR, write_data, 1, 0, true);

		CY_ASSERT(rslt == CY_RSLT_SUCCESS); /* If the write failed, halt the CPU */


		/* Use cyhal_i2c_master_read to read the required data from the device. 
		* The register address has already been set in the write above, so read two bytes
		* of data.
		*/

		rslt = cyhal_i2c_master_read(&i2c_monarch_obj, LTR_329_SUBORDINATE_ADDR, read_data, 4, 0, true);
		
		CY_ASSERT(rslt == CY_RSLT_SUCCESS); /* If the read failed, halt the CPU */


	
	}
	/* Return an 32-bit value that is the combination of ch1 & ch0, respectively. */

	return_value = read_data[0]; //first and lsb of data


	return return_value;
}

/** Read the value of the input port
 *
 * @param reg The reg address to read
 *
 */
int32_t LTR_329_get_light(void)
{
	int32_t raw_value = LTR_329_read_reg(LTR_329_LIGHT_REG_CH1);

	return raw_value;
}