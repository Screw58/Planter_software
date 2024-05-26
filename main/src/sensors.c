/*!
******************************************************************
* \file: sensors.c
* \date: 09.10.23
* \author: Jacek Twardoch
* \brief:
******************************************************************/

//======================================[INCLUDES]======================================//
#include "sensors.h"
#include "main.h"


//==================================[EXTERN VARIABLES]==================================//
extern GlobalErrorFlags_t GlobalErrorsTable;

//=============================[PRIVATE MACROS AND DEFINES]=============================//
#define TAG "app_sensors"

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
static esp_adc_cal_characteristics_t adc_chars;

//==================================[GLOBAL VARIABLES]==================================//

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//

/*!
 * \brief: Convert measured soil humidity value in [mV] to value in [%]
 * \details: Take value in mV, calculate it according to special formula,
 *           and return value in % [0-100]
 */
static uint8_t SoilSensCalc_mVtoPerc(const uint32_t SoilSensVolt_mV);

/*!
 * \brief: Trigger measurement of AHT15 sensor
 * \details: Send start measurement command to AHT15 through the I2C bus,
 *           Check if i2c bus error didn't occur
 */
static void Aht15StartMeasurement(const Aht15Sens_t *sensor);

/*!
 * \brief: Read measurements results from AHT15
 * \details: Send read commmand to AHT15 sensor, write these raw data into
 *           6 bytes buffer gave as parameter: 1 byte for status, 2.5 bytes for
 *           temperature and 2.5 bytes for humidity
 */
static void Aht15ReadResults(const Aht15Sens_t *sensor, uint32_t *temp_rawdata, uint32_t *hum_rawdata);

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
static void BH1750_StartMeasurement(const BH1750Sens_t *sensor);

/*!
 * \brief: Read measurement result from BH1750
 * \details: Send read command, put the data into 2 bytes buffer and afterwards convert
 *           and return uint16 rawdata which is illuminance value
 */
static uint16_t BH1750_ReadResult(const BH1750Sens_t *sensor);

/*!
 * \brief: Function reads data from I2C bus
 * \details: That function handles read data in a way described by Espressif documentation
 */
static void I2C_ReadData(const uint8_t addr, uint8_t *read_buf, const size_t data_len);

/*!
 * \brief: Function sends given data through I2C bus
 * \details: That function handles write data in a way described by Espressif documentation
 */
static void I2C_WriteData(const uint8_t addr, const uint8_t *write_buf, const size_t data_len);


//==================================[LOCAL FUNCTIONS]===================================//

static uint8_t SoilSensCalc_mVtoPerc(const uint32_t SoilSensVolt_mV)
{
   int SoilSensWithOffset_mV = 0;
   uint8_t SoilSensResult_perc = 0;

   SoilSensWithOffset_mV = SoilSensVolt_mV - SOIL_SENS_MEAS_OFFSET;
   SoilSensResult_perc = ((SOIL_MIN_VAL - SoilSensWithOffset_mV) * 100) / (SOIL_MIN_VAL - SOIL_MAX_VAL);

   return SoilSensResult_perc;
}

static void Aht15StartMeasurement(const Aht15Sens_t *sensor)
{
   uint8_t write_buf[3] = { AHT15_START_MEASUREMENT, AHT15_DATA_MEASUREMENT_CMD, AHT15_DATA_NOP };

   I2C_WriteData(sensor->address, write_buf, sizeof(write_buf));
}

static void Aht15ReadResults(const Aht15Sens_t *sensor, uint32_t *temp_rawdata, uint32_t *hum_rawdata)
{
   uint8_t data_buf[6];

   I2C_ReadData(sensor->address, data_buf, sizeof(data_buf));
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


static void BH1750_StartMeasurement(const BH1750Sens_t *sensor)
{
   uint8_t write_buf = BH1750_POWER_ON;
   I2C_WriteData(sensor->address, &write_buf, sizeof(write_buf));

   WAIT_IN_MS(100);
   /* BH1750FVI is not able to accept plural command without stop condition*/
   write_buf = BH1750_START_MEASUREMENT;
   I2C_WriteData(sensor->address, &write_buf, sizeof(write_buf));
}


static uint16_t BH1750_ReadResult(const BH1750Sens_t *sensor)
{
   uint8_t read_buf[2] = { 0 };
   uint16_t rawdata = 0;
   uint8_t write_buf = BH1750_POWER_ON;

   /* There is need to turn on the sensor cause of it's automatically turned down measurement is done */
   I2C_WriteData(sensor->address, &write_buf, sizeof(write_buf));

   I2C_ReadData(sensor->address, read_buf, sizeof(read_buf));

   rawdata = read_buf[0];
   rawdata = (rawdata << 8);
   rawdata |= read_buf[1];

   return rawdata;
}


static void I2C_ReadData(const uint8_t addr, uint8_t *read_buf, const size_t data_len)
{
   i2c_cmd_handle_t read_cmd_handle;
   esp_err_t i2c_status = ESP_OK;

   read_cmd_handle = i2c_cmd_link_create();
   i2c_master_start(read_cmd_handle);
   i2c_master_write_byte(read_cmd_handle, ((addr << 1) | I2C_MASTER_READ), ACK_ENABLE);
   if(data_len == 1)
   {
      i2c_master_read(read_cmd_handle, read_buf, 1, I2C_MASTER_LAST_NACK);
   }
   else
   {
      i2c_master_read(read_cmd_handle, read_buf, data_len - 1, I2C_MASTER_ACK);
      i2c_master_read(read_cmd_handle, read_buf + (data_len - 1), 1, I2C_MASTER_LAST_NACK);
   }

   i2c_master_stop(read_cmd_handle);
   i2c_status = i2c_master_cmd_begin(I2C_NUM_0, read_cmd_handle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
   i2c_cmd_link_delete(read_cmd_handle);
   I2cBusErrCheck(i2c_status);
}


static void I2C_WriteData(const uint8_t addr, const uint8_t *write_buf, const size_t data_len)
{
   i2c_cmd_handle_t write_cmd_handle;
   esp_err_t i2c_status = ESP_OK;

   write_cmd_handle = i2c_cmd_link_create();
   i2c_master_start(write_cmd_handle);
   i2c_master_write_byte(write_cmd_handle, ((addr << 1) | I2C_MASTER_WRITE), 1);
   i2c_master_write(write_cmd_handle, write_buf, data_len, ACK_ENABLE);
   i2c_master_stop(write_cmd_handle);
   i2c_status = i2c_master_cmd_begin(I2C_NUM_0, write_cmd_handle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
   i2c_cmd_link_delete(write_cmd_handle);
   I2cBusErrCheck(i2c_status);
}


//==================================[GLOBAL FUNCTIONS]==================================//

void ADC_Init(void)
{
   /*ADC calibration*/
   esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, ESP_ADC_CAL_VAL_EFUSE_VREF, &adc_chars);

   adc1_config_width(ADC_WIDTH_BIT_12);
   adc1_config_channel_atten(SOIL_SENS_ADC_CHANNEL, ADC_ATTEN_DB_11);
}

void I2C_Init(void)
{
   int i2c_master_port = 0;
   i2c_config_t i2c_config = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_SENSORS_SDA_PIN,
      .scl_io_num = I2C_SENSORS_SCL_PIN,
      .sda_pullup_en = PULLUP_ENABLE,
      .scl_pullup_en = PULLUP_ENABLE,
      .master.clk_speed = 400000,
   };

   i2c_param_config(I2C_NUM_0, &i2c_config);
   i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}

void SoilSensorReadResult(SoilSens_t *sensor)
{
   uint32_t AdcResultRawData = 0;
   uint32_t AdcResultVolt_mV = 0;
   uint8_t SoilMoistRes_perc = 0;

   // for(int i = 0; i <= SAMPLE_CNT; ++i)
   // {
   //    AdcResultRawData += adc1_get_raw(sensor->SensConfig.adc_channel);
   // }

   // AdcResultRawData /= SAMPLE_CNT;
   AdcResultRawData = adc1_get_raw(sensor->adc_channel);
   AdcResultVolt_mV = esp_adc_cal_raw_to_voltage(AdcResultRawData, &adc_chars);
   SoilMoistRes_perc = SoilSensCalc_mVtoPerc(AdcResultVolt_mV);

   sensor->soil_moisture = SoilMoistRes_perc;
   ESP_LOGI("adc_measurement", "adc voltage is equal to : %d ", AdcResultVolt_mV);
   if(SoilMoistRes_perc > 100)
   {
      GlobalErrorsTable.ErrorsStruct.SoilSensErr = ERROR_PRESENT;
   }
}

void Aht15SensInitSw(const Aht15Sens_t *sensor)
{
   uint8_t status_buf = 0;
   uint8_t write_buf[3] = { AHT15_INIT_CMD, AHT15_INIT_CAL_ENABLE, AHT15_INIT_NORMAL_MODE };

   I2C_WriteData(sensor->address, write_buf, sizeof(write_buf));

   /* wait for AHT15 sensor initialization */
   WAIT_IN_MS(350);
   // vTaskDelay(pdMS_TO_TICKS(350));

   I2C_ReadData(sensor->address, &status_buf, sizeof(status_buf));
   Aht15CheckStatusByte(status_buf);

   // ESP_LOGI("sensor_app","status bajt jest rowny = %X", data_buf);
}

void Aht15_TakeMeasurement(Aht15Sens_t *sensor)
{
   float humidity, temperature;
   uint32_t hum_rawdata, temp_rawdata;
   hum_rawdata = temp_rawdata = 0;

   Aht15StartMeasurement(sensor);

   WAIT_IN_MS(80);

   Aht15ReadResults(sensor, &temp_rawdata, &hum_rawdata);

   humidity = ((float)hum_rawdata / 1048576) * 100;
   temperature = ((float)temp_rawdata / 1048576) * 200 - 50;

   sensor->humidity = humidity;
   sensor->temperature = temperature;

   // ESP_LOGI("sensor_app", "humidity RAW DATA: %X , temperatura RAW DATA: %X ", hum_rawdata, temp_rawdata);
}


void BH1750_TakeMeasurement(BH1750Sens_t *sensor)
{
   uint16_t lux_rawdata = 0;

   BH1750_StartMeasurement(sensor);

   WAIT_IN_MS(BH1750_TIME_FOR_RESOLUTION);

   lux_rawdata = BH1750_ReadResult(sensor);

   lux_rawdata = (lux_rawdata * 10) / 12;
   sensor->illuminance = lux_rawdata;

   // ESP_LOGI("luxsensor", "ILOSC LUXOW WYNOSI:  %d (int)\nLUB ", lux_rawdata);
}
