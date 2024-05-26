/*!
******************************************************************
* \file: main.c
* \date: 09.10.23
* \author: Jacek Twardoch
* \brief: This file contains main function, wifi handler functions
          and function responsibles to put MCU to sleep
******************************************************************/

//======================================[INCLUDES]======================================//
#include "main.h"

//==================================[EXTERN VARIABLES]==================================//
extern bool aws_connected;
extern bool wifi_connected;

//=============================[PRIVATE MACROS AND DEFINES]=============================//
#define TAG "main_app"

/*These are defined various times for debug purposes*/
#define TWELVE_HOURS_IN_US 43200000000ULL
#define ONE_HOUR_IN_US     3600000000ULL
#define TEN_MINS_IN_US     600000000ULL
#define ONE_MINS_IN_US     60000000ULL

#define DEBUG

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//

//==================================[GLOBAL VARIABLES]==================================//
AllSensorsReadings_t AllSensorsReadings = { 0 };
//=============================[LOCAL FUNCTION PROTOTYPES]==============================//
/*!
 * \brief: Function which will be executed after successful wifi connection
 */
static void WifiConnectedHandler(void);

/*!
 * \brief: Function which will be executed after fail wifi connection
 */
static void WifiFailedHandler(void);

/*!
 * \brief: This function prepares and puts MCU to sleep
 * \details: Before sleep aws client is disconnected and wifi module is turned off
 */
static void GoToSleep(uint64_t time_in_us);

/*!
 * \brief: A function that combines all sensor measurements into one structure
 * \details: Then this structure is passed to function that send data to AWS database
 */
static void PrepareDataToSendAWS(const SoilSens_t *soilsensor, const Aht15Sens_t *aht15sensor, const BH1750Sens_t *lightsensor);

//==================================[LOCAL FUNCTIONS]===================================//

static void WifiConnectedHandler(void)
{
   ESP_LOGE(TAG, "Wifi succesfully connected!");
   wifi_connected = true;
   ConnectAwsMqtt();
}

static void WifiFailedHandler(void)
{
   ESP_LOGE(TAG, "Wifi failed");
}

static void PrepareDataToSendAWS(const SoilSens_t *soilsensor, const Aht15Sens_t *aht15sensor, const BH1750Sens_t *lightsensor)
{
   AllSensorsReadings.soil_moisture = soilsensor->soil_moisture;
   AllSensorsReadings.temperature = aht15sensor->temperature;
   AllSensorsReadings.humidity = aht15sensor->humidity;
   AllSensorsReadings.illuminance = lightsensor->illuminance;
}

static void GoToSleep(uint64_t time_in_us)
{
   AwsDisconnect();
   esp_wifi_stop();
   esp_sleep_enable_timer_wakeup(time_in_us);
   // esp_light_sleep_start();
   esp_deep_sleep_start();
}
//==================================[GLOBAL FUNCTIONS]==================================//

void app_main()
{
   ErrorId_t ErrId = 0;

   /* Create AHT15 sensor object */
   Aht15Sens_t aht15sensor1 = {
      .humidity = 0,
      .temperature = 0,
      .address = AHT15_ADDRESS,
   };

   /* Create soil moisture sensor object */
   SoilSens_t soilsensor1 = {
      .soil_moisture = 0,
      .adc_channel = SOIL_SENS_ADC_CHANNEL,
   };

   /* Create illuminance sensor object */
   BH1750Sens_t lightsensor1 = {
      .illuminance = 0,
      .address = BH1750_ADDRESS,
   };

   /* Callbacks for wifi connection */
   wifi_conn_params_f wifi_callback = {
      .on_connected = WifiConnectedHandler,
      .on_failed = WifiFailedHandler,
   };

   ESP_LOGE("debug_flag", "program_start");
   /* Time for aht15 startup */
   vTaskDelay(pdMS_TO_TICKS(60));

   /* Peripherals initialization */
   I2C_Init();
   ADC_Init();
   
   aws_connected = false;
   wifi_connected = false;

   AppWifiConnect(wifi_callback);
   vTaskDelay(pdMS_TO_TICKS(2000));
   Aht15SensInitSw(&aht15sensor1);

   while(1)
   {


     // vTaskDelay(pdMS_TO_TICKS(3000));

      // aws_connected = false;
      // wifi_connected = false;

      //      AppWifiConnect(wifi_callback);

      /* Wait until the AWS client connects to the server */
      ESP_LOGI("debug_flag", "before aws connected");
      while(!aws_connected)
      {
         vTaskDelay(pdMS_TO_TICKS(500));
      }
      ESP_LOGI("debug_flag", "before checking conditions");
      if((wifi_connected == true) && (aws_connected == true))
      {
         SoilSensorReadResult(&soilsensor1);
         Aht15_TakeMeasurement(&aht15sensor1);
         BH1750_TakeMeasurement(&lightsensor1);
         ErrId = CheckErrors();
         PrepareDataToSendAWS(&soilsensor1, &aht15sensor1, &lightsensor1);
         ESP_LOGI("debug_flag", "before publish to AWS");
         PublishReadingsToAws(&AllSensorsReadings, ErrId);
         ESP_LOGI("debug_flag", "after publish readings func");
      }

#ifdef DEBUG
      ESP_LOGI("sensor_app", "====================================");
      ESP_LOGI("sensor_app", "soil moisture: %d ", soilsensor1.soil_moisture);
      ESP_LOGI("sensor_app", "humidity: %f , temperature: %f ", aht15sensor1.humidity, aht15sensor1.temperature);
      ESP_LOGI("sensor_app", "illuminance: %d ", AllSensorsReadings.illuminance);
      ESP_LOGI("sensor_app", "Is WiFi connected: %s", wifi_connected == 1 ? "true" : "false");
      ESP_LOGI("sensor_app", "Is AWS connected: %s", aws_connected == 1 ? "true" : "false");
      ESP_LOGI("sensor_app", "====================================");
#endif
      ESP_LOGI("debug_flag", "before go to sleep");
      GoToSleep(ONE_MINS_IN_US * 3);
      //vTaskDelay(pdMS_TO_TICKS(10000));
      ESP_LOGI("debug_flag", "after sleep");
   }
}
