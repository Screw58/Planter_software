/*!
******************************************************************
* \file: sensors.c
* \date: 09.10.23
* \author: Jacek Twardoch
* \brief:
******************************************************************/

//======================================[INCLUDES]======================================//
#include "sensors.h"
#include "adc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "main.h"


//==================================[EXTERN VARIABLES]==================================//
extern GlobalErrorFlags_t GlobalErrorsTable;
extern bool AreMeasurementsReady;
//=============================[PRIVATE MACROS AND DEFINES]=============================//
#define TAG_SNS "app_sensors"

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
// static esp_adc_cal_characteristics_t adc_chars;

/* Create soil moisture sensor object */
static SoilSens_t soilsensor1;

/* Create humidity and temperature sensor object */
static Aht15Sens_t aht15sensor1;

/* Create illuminance sensor object */
static BH1750Sens_t lightsensor1;

//==================================[GLOBAL VARIABLES]==================================//

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//

/*!
 * \brief: Convert measured soil humidity value in [mV] to value in [%]
 * \details: Take value in mV, calculate it according to special formula,
 *           and return value in % [0-100]
 */
static uint8_t SoilSensCalc_mVtoPerc(const int SoilSensVolt_mV);

/*!
 * \brief: Trigger measurement of AHT15 sensor
 * \details: Send start measurement command to AHT15 through the I2C bus,
 *           Check if i2c bus error didn't occur
 */
static SensErrType_t Aht15StartMeasurement(const Aht15Sens_t *sensor);

/*!
 * \brief: Read measurements results from AHT15
 * \details: Send read commmand to AHT15 sensor, write these raw data into
 *           6 bytes buffer gave as parameter: 1 byte for status, 2.5 bytes for
 *           temperature and 2.5 bytes for humidity
 */
static SensErrType_t Aht15ReadResults(Aht15Sens_t *sensor, uint32_t *temp_rawdata, uint32_t *hum_rawdata);

/*!
 * \brief: Check if any error of AHT15 sensor occured
 * \details: Two bits of first answer byte are checking and errors are setting:
 *           -If AHT15 busy bit is active - results can be unreliable
 *           -If AHT15 calib bit is inactive - sensor is not calibrated
 */
static void Aht15CheckStatusByte(const uint8_t stsbyte);

/*!
 * \brief: Trigger measurement of BH1750 sensor
 * \details : Power on the sensor, send start measurement command to BH1750 through the I2C bus,
 *           Check if i2c bus error didn't occur
 */
static SensErrType_t BH1750_StartMeasurement(BH1750Sens_t *sensor);

/*!
 * \brief: Read measurement result from BH1750
 * \details: Send read command, put the data into 2 bytes buffer and afterwards convert
 *           and return uint16 rawdata which is illuminance value
 */
static SensErrType_t BH1750_ReadResult(BH1750Sens_t *sensor, uint16_t *const lux_rawdata);

/*!
 * \brief: Function reads measure value of soil humidity
 * \details: -Read raw data from ADC channel
 *           -Convert that data to voltage value
 *           -Write result do sensor's structure
 *           -Check if value is in range
 *              -If not - set error occurence
 */
static void SoilSensorReadResult(SoilSens_t *sensor);

/*!
 * \brief: Initialize AHT15 sensor by sending command
 * \details: Following steps are performed:
 *           -Send command to AHT15 to initialize and calibrate
 *           -Check if I2C bus error occured
 *           -Check status byte if init was succesfully
 */
static void Aht15SensInitSw(Aht15Sens_t *sensor);

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
static void Aht15_TakeMeasurement(Aht15Sens_t *sensor);

/*!
 * \brief: Take illuminance measurement of BH1750 sensor
 * \details:
 */
static void BH1750_TakeMeasurement(BH1750Sens_t *sensor);

//==================================[LOCAL FUNCTIONS]===================================//
static void SoilSensorReadResult(SoilSens_t *sensor)
{
   esp_err_t EspErr = ESP_OK;
   int AdcResultVolt_mV = 0;
   uint8_t SoilMoistRes_perc = 0;

   // for(int i = 0; i <= SAMPLE_CNT; ++i)
   // {
   //    AdcResultRawData += adc1_get_raw(sensor->SensConfig.adc_channel);
   // }

   // AdcResultRawData /= SAMPLE_CNT;
   EspErr = ADC_Read_mV(sensor->adc_channel, &AdcResultVolt_mV);
   SoilMoistRes_perc = SoilSensCalc_mVtoPerc(AdcResultVolt_mV);

   sensor->soil_moisture = SoilMoistRes_perc;
   ESP_LOGI("adc_measurement", "adc voltage is equal to : %d ", AdcResultVolt_mV);
   if(SoilMoistRes_perc > 100)
   {
      GlobalErrorsTable.ErrorsStruct.SoilSensErr = ERROR_PRESENT;
   }

   if(EspErr != ESP_OK)
   {
      sensor->SoilSensErr = ERROR_PRESENT;
   }
   else
   {
      /* Do nothing */
   }
}

static uint8_t SoilSensCalc_mVtoPerc(const int SoilSensVolt_mV)
{
   int SoilSensWithOffset_mV = 0;
   uint8_t SoilSensResult_perc = 0;

   // !< TODO - check if SoilSensVolt_mV is positive, if negative - start alert
   SoilSensWithOffset_mV = SoilSensVolt_mV - SOIL_SENS_MEAS_OFFSET;
   SoilSensResult_perc = ((SOIL_MIN_VAL - SoilSensWithOffset_mV) * 100) / (SOIL_MIN_VAL - SOIL_MAX_VAL);

   return SoilSensResult_perc;
}

static void Aht15SensInitSw(Aht15Sens_t *sensor)
{
   uint8_t status_buf = 0;
   uint8_t write_buf[3] = { AHT15_INIT_CMD, AHT15_INIT_CAL_ENABLE, AHT15_INIT_NORMAL_MODE };

   I2C_WriteData(sensor->i2c_dev_handle, write_buf, sizeof(write_buf));

   /* wait for AHT15 sensor initialization */
   WAIT_IN_MS(350);
   // vTaskDelay(pdMS_TO_TICKS(350));

   I2C_ReadData(sensor->i2c_dev_handle, &status_buf, sizeof(status_buf));
   Aht15CheckStatusByte(status_buf);

   // ESP_LOGI("sensor_app","status bajt jest rowny = %X", data_buf);
}

static void Aht15CheckStatusByte(const uint8_t statusbyte)
{
   if(statusbyte & AHT15_BUSY_BIT_MASK)
   {
      GlobalErrorsTable.ErrorsStruct.Aht15BusyErr = ERROR_PRESENT;
   }

   if(!(statusbyte & AHT15_CALIB_BIT_MASK))
   {
      GlobalErrorsTable.ErrorsStruct.Aht15CalibErr = ERROR_PRESENT;
   }

   // ESP_LOGI("sensors_file", "RAW DATA OF STS BYTE = %x", statusbyte);
}

static SensErrType_t Aht15StartMeasurement(const Aht15Sens_t *sensor)
{
   esp_err_t EspErr = ESP_FAIL;
   SensErrType_t SensErr = ERROR_NOT_PRESENT;
   uint8_t write_buf[3] = { AHT15_START_MEASUREMENT, AHT15_DATA_MEASUREMENT_CMD, AHT15_DATA_NOP };

   EspErr = I2C_WriteData(sensor->i2c_dev_handle, write_buf, sizeof(write_buf));

   if(EspErr != ESP_OK)
   {
      SensErr = ERROR_PRESENT;
   }
   else
   {
      /* Do nothing */
   }

   return SensErr;
}

static SensErrType_t Aht15ReadResults(Aht15Sens_t *sensor, uint32_t *temp_rawdata, uint32_t *hum_rawdata)
{
   uint8_t data_buf[6];
   esp_err_t EspErr = ESP_FAIL;
   SensErrType_t SensErr = ERROR_NOT_PRESENT;

   EspErr = I2C_ReadData(sensor->i2c_dev_handle, data_buf, sizeof(data_buf));
   Aht15CheckStatusByte(data_buf[0]);

   *hum_rawdata = data_buf[1];
   *hum_rawdata = (*hum_rawdata << 8);
   *hum_rawdata |= data_buf[2];
   *hum_rawdata = (*hum_rawdata << 8);
   *hum_rawdata |= data_buf[3];
   *hum_rawdata = (*hum_rawdata >> 4);

   *temp_rawdata = (data_buf[3] & 0x0F);
   *temp_rawdata = (*temp_rawdata << 8);
   *temp_rawdata |= data_buf[4];
   *temp_rawdata = (*temp_rawdata << 8);
   *temp_rawdata |= data_buf[5];

   if(EspErr != ESP_OK)
   {
      SensErr = ERROR_PRESENT;
   }
   else
   {
      /* Do nothing */
   }

   return SensErr;
}


static void Aht15_TakeMeasurement(Aht15Sens_t *sensor)
{
   float humidity, temperature;
   uint32_t hum_rawdata, temp_rawdata;
   SensErrType_t SensErr = ERROR_NOT_PRESENT;

   hum_rawdata = temp_rawdata = 0;

   SensErr |= Aht15StartMeasurement(sensor);

   WAIT_IN_MS(80);

   SensErr |= Aht15ReadResults(sensor, &temp_rawdata, &hum_rawdata);

   humidity = ((float)hum_rawdata / 1048576) * 100;
   temperature = ((float)temp_rawdata / 1048576) * 200 - 50;

   sensor->humidity = humidity;
   sensor->temperature = temperature;

   if(SensErr == ERROR_PRESENT)
   {
      sensor->Aht15SensErr = ERROR_PRESENT;
   }

   ESP_LOGI("sensor_app", "humidity RAW DATA: %lu , temperatura RAW DATA: %lu ", hum_rawdata, temp_rawdata);
   ESP_LOGI("sensor_app", "humidity: %f , temperatura: %f ", humidity, temperature);
}

static SensErrType_t BH1750_StartMeasurement(BH1750Sens_t *sensor)
{
   uint8_t write_buf = BH1750_POWER_ON;
   SensErrType_t SensErr = ERROR_NOT_PRESENT;
   esp_err_t EspErr = ESP_OK;

   EspErr |= I2C_WriteData(sensor->i2c_dev_handle, &write_buf, sizeof(write_buf));

   WAIT_IN_MS(100);
   /* BH1750FVI is not able to accept plural command without stop condition*/
   write_buf = BH1750_START_MEASUREMENT;
   EspErr |= I2C_WriteData(sensor->i2c_dev_handle, &write_buf, sizeof(write_buf));

   if(EspErr != ESP_OK)
   {
      SensErr = ERROR_PRESENT;
   }
   else
   {
      /* Do nothing */
   }

   return SensErr;
}


static SensErrType_t BH1750_ReadResult(BH1750Sens_t *sensor, uint16_t *const lux_rawdata)
{
   uint8_t read_buf[2] = { 0 };
   uint16_t rawdata = 0;
   SensErrType_t SensErr = ERROR_NOT_PRESENT;
   esp_err_t EspErr = ESP_OK;

   uint8_t write_buf = BH1750_POWER_ON;

   /* There is need to turn on the sensor cause of it's automatically turned down measurement is done */
   EspErr |= I2C_WriteData(sensor->i2c_dev_handle, &write_buf, sizeof(write_buf));

   EspErr |= I2C_ReadData(sensor->i2c_dev_handle, read_buf, sizeof(read_buf));

   rawdata = read_buf[0];
   rawdata = (rawdata << 8);
   rawdata |= read_buf[1];

   *lux_rawdata = rawdata;

   if(EspErr != ESP_OK)
   {
      SensErr = ERROR_PRESENT;
   }
   else
   {
      /* Do nothing */
   }

   return SensErr;
}


static void BH1750_TakeMeasurement(BH1750Sens_t *sensor)
{
   uint16_t lux_rawdata = 0;
   SensErrType_t SensErr = ERROR_NOT_PRESENT;

   SensErr |= BH1750_StartMeasurement(sensor);

   WAIT_IN_MS(BH1750_TIME_FOR_RESOLUTION);

   SensErr |= BH1750_ReadResult(sensor, &lux_rawdata);

   lux_rawdata = (lux_rawdata * 10) / 12;
   sensor->illuminance = lux_rawdata;

   if(SensErr == ERROR_PRESENT)
   {
      sensor->BH1750SensErr = ERROR_PRESENT;
   }

   ESP_LOGI("luxsensor", "ILOSC LUXOW WYNOSI:  %d (int)\nLUB ", lux_rawdata);
}
//==================================[GLOBAL FUNCTIONS]==================================//

void SensorsInit(void)
{
   /* Initialize soil moisture sensor */
   soilsensor1.soil_moisture = 0;
   soilsensor1.adc_channel = SOILSENS_ADC_CHANNEL;
   soilsensor1.SoilSensErr = ERROR_NOT_PRESENT;
   ADC_ConfigChannel(soilsensor1.adc_channel);

   /* Initialize AHT15 sensor */
   aht15sensor1.humidity = 0;
   aht15sensor1.temperature = 0;
   aht15sensor1.i2caddr = AHT15_ADDRESS;
   aht15sensor1.i2c_dev_handle = 0;
   aht15sensor1.Aht15SensErr = ERROR_NOT_PRESENT;
   I2C_Add_device_to_I2Cbus(&(aht15sensor1.i2c_dev_handle), aht15sensor1.i2caddr);

   /* Initialize illuminance sensor */
   lightsensor1.illuminance = 0;
   lightsensor1.i2caddr = BH1750_ADDRESS;
   lightsensor1.i2c_dev_handle = 0;
   lightsensor1.BH1750SensErr = ERROR_NOT_PRESENT;
   I2C_Add_device_to_I2Cbus(&(lightsensor1.i2c_dev_handle), lightsensor1.i2caddr);

   Aht15SensInitSw(&aht15sensor1);
}


void TakeSensorMeasurements(AllSensorsReadings_t *sensors_readings)
{
   SoilSensorReadResult(&soilsensor1);
   Aht15_TakeMeasurement(&aht15sensor1);
   BH1750_TakeMeasurement(&lightsensor1);

   vTaskDelay(pdMS_TO_TICKS(2000));
   sensors_readings->soil_moisture = soilsensor1.soil_moisture;
   sensors_readings->temperature = aht15sensor1.temperature;
   sensors_readings->humidity = aht15sensor1.humidity;
   sensors_readings->illuminance = lightsensor1.illuminance;
   // AreMeasurementsReady = true;
}

void CheckSensorErrors(AllSensorsErrors_t *sensor_errors)
{
   sensor_errors->SoilSensErr = soilsensor1.SoilSensErr;
   sensor_errors->Aht15SensErr = aht15sensor1.Aht15SensErr;
   sensor_errors->BH1750SensErr = lightsensor1.BH1750SensErr;
}