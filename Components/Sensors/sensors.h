/*!
*******************************************************
* \file: sensors.h
* \date: 09.10.23
* \author: Jacek Twardoch
* \brief:
********************************************************/
#ifndef SENSORS_H_
#define SENSORS_H_

//======================================[INCLUDES]======================================//
// #include ""
#include "adc.h"
#include "i2c.h"

//=================================[MACROS AND DEFINES]=================================//

/* Defines for I2C bus */

#define I2C_SENSORS_SDA_PIN GPIO_NUM_21
#define I2C_SENSORS_SCL_PIN GPIO_NUM_22
#define PULLUP_ENABLE       1
#define PULLUP_DISABLE      0
#define ACK_ENABLE          1
#define ACK_DISABLE         0

/* Defines for AHT15 sensor */
#define AHT15_ADDRESS              0x38
#define AHT15_INIT_CMD             0xE1
#define AHT15_INIT_NORMAL_MODE     0x00
#define AHT15_INIT_CAL_ENABLE      0x08

#define AHT15_START_MEASUREMENT    0xAC
#define AHT15_DATA_MEASUREMENT_CMD 0x33
#define AHT15_DATA_NOP             0x00
#define AHT15_SOFT_RESET           0xBA

#define AHT15_BUSY_BIT_MASK        0x80
#define AHT15_CALIB_BIT_MASK       0x08

/* Defines for soil humidity sensor */
#define SOIL_SENS_ADC_CHANNEL ADC1_CHANNEL_6
#define SOIL_SENS_MEAS_OFFSET 200
#define SOIL_MAX_VAL          1400
#define SOIL_MIN_VAL          2000

/* Defines for BH1750 sensor */
#define BH1750_ADDRESS           0x23
#define BH1750_POWER_ON          0x01
#define BH1750_START_MEASUREMENT 0x23  // One-time L-resolution mode
// #define BH1750_START_MEASUREMENT   0x20  // One-time H-resolution mode
#define BH1750_TIME_FOR_RESOLUTION 30

//======================================[TYPEDEFS]======================================//
typedef uint8_t i2c_addr_t;
typedef float temp_celsius_t;
typedef float hum_percnt_t;
typedef uint8_t soilmoist_percnt_t;
typedef uint32_t illum_lux_t;

typedef struct {
   adc_channel_t adc_channel;
   soilmoist_percnt_t soil_moisture;
} SoilSens_t;

typedef struct {
   hum_percnt_t humidity;
   temp_celsius_t temperature;
   i2c_addr_t i2caddr;
   i2c_master_dev_handle_t i2c_dev_handle;
} Aht15Sens_t;

typedef struct {
   illum_lux_t illuminance;
   i2c_addr_t i2caddr;
   i2c_master_dev_handle_t i2c_dev_handle;
} BH1750Sens_t;

typedef struct {
   soilmoist_percnt_t soil_moisture;
   hum_percnt_t humidity;
   temp_celsius_t temperature;
   illum_lux_t illuminance;
} AllSensorsReadings_t;

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
void SensorsInit(void);
void TakeSensorMeasurements(AllSensorsReadings_t *sensors_readings);

#endif