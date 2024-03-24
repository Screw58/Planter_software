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
#include "driver/adc.h"
#include "driver/i2c.h"
#include "esp_adc_cal.h"

//=================================[MACROS AND DEFINES]=================================//

/* Defines for I2C bus */
#define I2C_MASTER_TIMEOUT_MS 1000
#define I2C_SENSORS_SDA_PIN   GPIO_NUM_21
#define I2C_SENSORS_SCL_PIN   GPIO_NUM_22
#define PULLUP_ENABLE         1
#define PULLUP_DISABLE        0
#define ACK_ENABLE            1
#define ACK_DISABLE           0

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

typedef struct {
   adc1_channel_t adc_channel;
   uint8_t soil_moisture;
} SoilSens_t;

typedef struct {
   float humidity;
   float temperature;
   i2c_addr_t address;
} Aht15Sens_t;

typedef struct {
   uint32_t illuminance;
   i2c_addr_t address;
} BH1750Sens_t;

typedef struct {
   uint8_t soil_moisture;
   float humidity;
   float temperature;
   uint32_t illuminance;
} AllSensorsReadings_t;

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//

/*!
 * \brief: Initialize ADC channel parameters for handling soil sensor
 */
void ADC_Init(void);

/*!
 * \brief: Initialize uC peripherals for AHT15 sensor handling
 * \details: Set up gpio pins for i2c communication
 */
void I2C_Init(void);

/*!
 * \brief: Function reads measure value of soil humidity
 * \details: -Read raw data from ADC channel
 *           -Convert that data to voltage value
 *           -Write result do sensor's structure
 *           -Check if value is in range
 *              -If not - set error occurence
 */
void SoilSensorReadResult(SoilSens_t *sensor);

/*!
 * \brief: Initialize AHT15 sensor by sending command
 * \details: Following steps are performed:
 *           -Send command to AHT15 to initialize and calibrate
 *           -Check if I2C bus error occured
 *           -Check status byte if init was succesfully
 */
void Aht15SensInitSw(const Aht15Sens_t *sensor);

/*!
 * \brief: Take temperature and humidity measurements of AHT15 sensor
 * \details: Following steps are performed:
 *           -Send start measurement command on I2C bus to AHT15
 *           -Wait until measurement will be done (time from sensor datasheet)
 *           -Read results
 *           -Check status byte if error occur
 *           -Calculate data to physical values: temp[C] and humidity [%]
 *           -Write results to sensor's structure
 */
void Aht15_TakeMeasurement(Aht15Sens_t *sensor);

/*!
 * \brief: Take illuminance measurement of BH1750 sensor
 * \details:
 */
void BH1750_TakeMeasurement(BH1750Sens_t *sensor);

#endif