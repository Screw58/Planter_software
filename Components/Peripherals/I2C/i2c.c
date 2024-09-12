/*!
******************************************************************
* \file:i2c.c
* \date: 12.08.2024
* \author: Jacek Twardoch
* \brief:
******************************************************************/

//======================================[INCLUDES]======================================//
#include "i2c.h"
//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
i2c_master_bus_handle_t i2c_bus_handle;

//==================================[GLOBAL VARIABLES]==================================//

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//

//==================================[LOCAL FUNCTIONS]===================================//

//==================================[GLOBAL FUNCTIONS]==================================//
void I2C_Init(void)
{
   i2c_master_bus_config_t i2c_bus_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .i2c_port = TEST_I2C_PORT,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,
   };

   ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle));
}

void I2C_Add_device_to_I2Cbus(i2c_master_dev_handle_t *const i2c_dev_handle, const uint8_t addr)
{
   i2c_device_config_t dev_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = addr,
      .scl_speed_hz = 100000,
   };

   if(i2c_dev_handle != NULL)
   {
      ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg, i2c_dev_handle));
   }
}

void I2C_ReadData(i2c_master_dev_handle_t dev_handle, uint8_t *read_buf, const size_t data_len)
{
   if((dev_handle != NULL) && (read_buf != NULL))
   {
      ESP_ERROR_CHECK(i2c_master_receive(dev_handle, read_buf, data_len, I2C_MASTER_TIMEOUT_MS));
   }
}


void I2C_WriteData(i2c_master_dev_handle_t dev_handle, const uint8_t *write_buf, const size_t data_len)
{
   if((dev_handle != NULL) && (write_buf != NULL))
   {
      ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, write_buf, data_len, I2C_MASTER_TIMEOUT_MS));
   }
}