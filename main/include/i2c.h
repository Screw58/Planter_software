/*!
*******************************************************
* \file: i2c.h
* \date: 12.08.2024
* \author: Jacek Twardoch
* \brief:
********************************************************/
#ifndef I2C_H_
#define I2C_H_

//======================================[INCLUDES]======================================//
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_err.h"

//=================================[MACROS AND DEFINES]=================================//
#define I2C_MASTER_SDA_IO     GPIO_NUM_21
#define I2C_MASTER_SCL_IO     GPIO_NUM_22
#define I2C_MASTER_TIMEOUT_MS 1000
#define TEST_I2C_PORT         0

//======================================[TYPEDEFS]======================================//

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//

/*!
 * \brief: Initialize uC peripherals for  handling
 * \details: Set up gpio pins for i2c communication
 */
void I2C_Init(void);

/*!
 *
 */
void I2C_Add_device_to_I2Cbus(i2c_master_dev_handle_t *const i2c_dev_handle, const uint8_t addr);

/*!
 * \brief: Function reads data from I2C bus
 * \details: That function handles read data in a way described by Espressif documentation
 */
void I2C_ReadData(i2c_master_dev_handle_t dev_handle, uint8_t *read_buf, const size_t data_len);

/*!
 * \brief: Function sends given data through I2C bus
 * \details: That function handles write data in a way described by Espressif documentation
 */
void I2C_WriteData(i2c_master_dev_handle_t dev_handle, const uint8_t *write_buf, const size_t data_len);

#endif