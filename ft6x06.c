#include "ft6x06.h"

//*****************************************************************************
// Sets the address to read/write from in the FT6x06  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    8-bit address
//
// Returns
// I2C_OK if the byte was written to the FT6X06.
//*****************************************************************************
static cy_rslt_t ft6x06_set_addr
( 
  uint8_t  address
)
{
  cy_rslt_t rslt;

  uint8_t write_buffer[] = {address};

  rslt = cyhal_i2c_master_write(
							  &i2c_monarch_obj,
							  FT6X06_I2C_ADDR,	// I2C Address
							  write_buffer, 		// Array of data to write
							  1, 								// Number of bytes to write
							  0, 								// Block until completed
							  true);					  // Generate Stop Condition

  return rslt;
}

  
  
//*****************************************************************************
// Writes one byte of data to the address specified for the FT6x06  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    8-bit address
//
//    data:       data to write.
//
// Returns
//*****************************************************************************
static cy_rslt_t ft6x06_write_data
( 
  uint8_t  address,
  uint8_t data
)
{
  cy_rslt_t rslt; 
  uint8_t write_buffer[] = {address, data};
 
  rslt = cyhal_i2c_master_write(
							  &i2c_monarch_obj,
							  FT6X06_I2C_ADDR,	// I2C Address
							  write_buffer, 		// Array of data to write
							  2, 								// Number of bytes to write
							  0, 								// Block until completed
							  true);						// Generate Stop Condition

  return rslt;
}

//*****************************************************************************
// Reads one byte of data from the current address of the FT6x06  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    data:       data to read.
//
// Returns
// I2C_OK if the byte was written to the FT6X06.
//*****************************************************************************
static cy_rslt_t ft6x06_read_data
( 
  uint8_t *data
)
{
  cy_rslt_t rslt; 
  uint8_t read_buffer[1];
 
  rslt = cyhal_i2c_master_read(
							  &i2c_monarch_obj,
							  FT6X06_I2C_ADDR,	// I2C Address
							  read_buffer, 		// Array of data to write
							  1, 								// Number of bytes to write
							  0, 								// Block until completed
							  true);						// Generate Stop Condition

  *data = read_buffer[0];

  return rslt;

}


//*****************************************************************************
// Read the number of active touch points.
//*****************************************************************************
uint8_t ft6x06_read_td_status(void)
{ 
  uint8_t td_status;
  
  ft6x06_set_addr(FT6X06_TD_STATUS_R);
  
  ft6x06_read_data(&td_status);

  if( td_status == 1 || td_status == 2)
  {
    return td_status;
  }
  else
  {
    return 0;
  }
  
} 


/* Upper left is X 201, Y 27 */
/* Upper right is X 201, Y 310 */
/* Lower left is X 45, Y 32 */
/* Lower right is X 45, Y 310 */

//*****************************************************************************
// Read the X value of last touch event
// 
// The LCD screen is rotated on the ECE353 Development boards so that
// the pixels are in a 320x240 (rows, columns).  In order to correctly
// get the X direction, we need to read the X register from the Captouch
// sensor and subtract this value from 240.
//*****************************************************************************
uint16_t ft6x06_read_y(void)
{ 
  uint8_t hi;
  uint8_t low;
  
  ft6x06_set_addr(FT6X06_P1_XH_R);
  
  ft6x06_read_data(&hi);
  
  ft6x06_set_addr(FT6X06_P1_XL_R);
  
  ft6x06_read_data(&low);
  
  return 240-((hi & 0x0F) << 8 | low);
} 

//*****************************************************************************
// Read the Y value of last touch event
// The LCD screen is rotated on the ECE353 Development boards so that
// the pixels are in a 320x240 (rows, columns).  In order to correctly
// get the X direction, we need to read the Y register from the Captouch
// sensor.
//*****************************************************************************
uint16_t ft6x06_read_x(void)
{ 
  uint8_t hi;
  uint8_t low;
  
  
  ft6x06_set_addr(FT6X06_P1_YH_R);
  
  ft6x06_read_data(&hi);
  
  ft6x06_set_addr(FT6X06_P1_YL_R);
  
  ft6x06_read_data(&low);
  

  return (hi & 0x0F) << 8 | low;
} 


//*****************************************************************************
// Test the ft6x06
//*****************************************************************************
void test_ft6x06(void)
{
  cy_rslt_t passed;
  
  uint8_t id;
  
  passed = ft6x06_set_addr(FT6X06_FOCALTECH_ID_R);
  
	printf("==== Starting ft6x06 Test ====\n\r");
  if(  passed != CY_RSLT_SUCCESS)
  {
    printf("I2C Bus Error\n\r");
  }
  else
  {
    
    passed = ft6x06_read_data(&id);
    
    // Print if the test passed or failed.
    if ( (passed == CY_RSLT_SUCCESS) && (id == 0x11))
    {
      printf("\ft6x06 Detected: 0x%02x\n\r", id);
    }
    else if ( (passed == CY_RSLT_SUCCESS) && (id != 0x11))
    {
      printf("\ft6x06 ID code mismatch: 0x%02x\n\r",id);
    }
    else
    {
      printf("I2C Bus Error\n\r");
    }
  }
  
		printf("==== Stopping ft6x06 Test ====\n\n\r");
}

