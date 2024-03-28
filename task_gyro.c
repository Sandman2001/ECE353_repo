/**
 * @file task_gyro.c
 * @author Zachary Gunderson (ztgunderson@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_gyro.h"

QueueHandle_t Queue_Gyro_X_Position;
QueueHandle_t Queue_Gyro_Y_Position;


/**
 * @brief This task receives data from lsm6dsm gyro and sends to a queue
 * @param pvParameters 
 */
void task_gyro(void *pvParameters){
    uint8_t gyro_buffer_x[4];
    uint8_t gyro_buffer_y[4];
    /* Initialize mems driver interface */
    stmdev_ctx_t dev_ctx;
    lsm6dsm_int1_route_t int_1_reg;
    /* Uncomment if interrupt generation on 6D INT2 pin */
    // lsm6dsm_int2_route_t int_2_reg;
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;

    /* Init test platform */
    platform_init();
    /* Wait sensor boot time */
    platform_delay(15);
    /* Check device ID */
    lsm6dsm_device_id_get(&dev_ctx, &whoamI);

    if (whoamI != LSM6DSM_ID)
    {
        printf("Device not found\n\r");
    while (1)
    {
    /* manage here device not found */
    }
    }

    /* Restore default configuration */
    lsm6dsm_reset_set(&dev_ctx, PROPERTY_ENABLE);

    do
    {
        lsm6dsm_reset_get(&dev_ctx, &rst);
    } while (rst);

        /* Set XL Output Data Rate */
        lsm6dsm_xl_data_rate_set(&dev_ctx, LSM6DSM_XL_ODR_416Hz);
        /* Set 2g full XL scale */
        lsm6dsm_xl_full_scale_set(&dev_ctx, LSM6DSM_2g);
        /* Set threshold to 60 degrees */
        lsm6dsm_6d_threshold_set(&dev_ctx, LSM6DSM_DEG_60);
        /* Use HP filter */
        lsm6dsm_xl_hp_path_internal_set(&dev_ctx, LSM6DSM_USE_HPF);
        /* LPF2 on 6D function selection */
        lsm6dsm_6d_feed_data_set(&dev_ctx, LSM6DSM_LPF2_FEED);
        /* Enable interrupt generation on 6D INT1 pin */
        lsm6dsm_pin_int1_route_get(&dev_ctx, &int_1_reg);
        int_1_reg.int1_6d = PROPERTY_ENABLE;
        lsm6dsm_pin_int1_route_set(&dev_ctx, int_1_reg);

    while(1){
        lsm6dsm_read_reg(&dev_ctx, IMU_REG_OUTX_H_XL, gyro_buffer_x, 1);
        xQueueSend(Queue_Gyro_X_Position, &gyro_buffer_x[0], portMAX_DELAY);
        vTaskDelay(10);
        lsm6dsm_read_reg(&dev_ctx, IMU_REG_OUTY_H_XL, gyro_buffer_y, 1);
        xQueueSend(Queue_Gyro_Y_Position, &gyro_buffer_y[0], portMAX_DELAY);
        vTaskDelay(10);
        //printf("x val: %d, y val: %d \n", gyro_buffer_x[0], gyro_buffer_y[0]);
        
    }

}


