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
#include "esp_log.h"

//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//
#define TAG_MAIN "main_app"

/*These are defined various times for debug purposes*/
#define TWELVE_HOURS_IN_US 43200000000ULL
#define ONE_HOUR_IN_US     3600000000ULL
#define TEN_MINS_IN_US     600000000ULL
#define ONE_MINS_IN_US     60000000ULL

#define DEBUG
//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
static volatile AllSensorsReadings_t AllSensorsReadings = { 0 };

//==================================[GLOBAL VARIABLES]==================================//
// AllSensorsReadings_t AllSensorsReadings = { 0 };
wifi_conn_params_f wifi_callback;
bool wifi_connected = false;
bool mqtt_connected = false;
bool AreMeasurementsReady = false;

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//
/*!
 * \brief: Function which will be executed after successful wifi connection
 */
void WifiConnectedHandler(void);

/*!
 * \brief: Function which will be executed after fail wifi connection
 */
void WifiFailedHandler(void);

/*!
 * \brief: This function prepares and puts MCU to sleep
 * \details: Before sleep aws client is disconnected and wifi module is turned off
 */
static void GoToSleep(uint64_t time_in_us);

/*!
 * \brief: A function that combines all sensor measurements into one structure
 * \details: Then this structure is passed to function that send data to AWS database
 */
// static void PrepareDataToSendAWS(const SoilSens_t *soilsensor, const Aht15Sens_t *aht15sensor, const BH1750Sens_t *lightsensor);

//==================================[LOCAL FUNCTIONS]===================================//


// static void PrepareDataToSendAWS(const SoilSens_t *soilsensor, const Aht15Sens_t *aht15sensor, const BH1750Sens_t *lightsensor)
// {
//    AllSensorsReadings.soil_moisture = soilsensor->soil_moisture;
//    AllSensorsReadings.temperature = aht15sensor->temperature;
//    AllSensorsReadings.humidity = aht15sensor->humidity;
//    AllSensorsReadings.illuminance = lightsensor->illuminance;
// }
void WifiFailedHandler(void)
{
   ESP_LOGE(TAG_MAIN, "Wifi failed");
}

void WifiConnectedHandler(void)
{
   ESP_LOGE(TAG_MAIN, "Wifi succesfully connected!");
   wifi_connected = true;

   Mqtt_Init();
}

static void GoToSleep(uint64_t time_in_us)  // TODO
{
   // AwsDisconnect();
   esp_wifi_stop();
   esp_sleep_enable_timer_wakeup(time_in_us);
   // esp_light_sleep_start();
   esp_deep_sleep_start();
}

// void HardwareInit(void)
// {
//    /* Peripherals initialization */
//    ADC_Init();
//    I2C_Init();
// }
//==================================[GLOBAL FUNCTIONS]==================================//

void app_main()
{
   ErrorId_t ErrId = 0;


   wifi_callback.on_connected = WifiConnectedHandler;
   wifi_callback.on_failed = WifiFailedHandler;

   // HardwareInit();
   ADC_Init();
   I2C_Init();
   SensorsInit();
   ESP_LOGE("debug_flag", "program_start");

   /* Time for aht15 startup */
   vTaskDelay(pdMS_TO_TICKS(60));

   mqtt_connected = false;
   wifi_connected = false;

   AppWifiConnect(wifi_callback);
   vTaskDelay(pdMS_TO_TICKS(2000));


   while(1)
   {


      // vTaskDelay(pdMS_TO_TICKS(3000));

      // aws_connected = false;
      // wifi_connected = false;

      //      AppWifiConnect(wifi_callback);

      /* Wait until the AWS client connects to the server */
      ESP_LOGI("debug_flag", "before aws connected");
      while(!mqtt_connected)
      {
         vTaskDelay(pdMS_TO_TICKS(500));
      }
      ESP_LOGI("debug_flag", "before checking conditions");
      if((wifi_connected == true) && (mqtt_connected == true))
      {
         AreMeasurementsReady = false;
         TakeSensorMeasurements(&AllSensorsReadings);
         ErrId = CheckErrors();
         // PrepareDataToSendAWS(&soilsensor1, &aht15sensor1, &lightsensor1);
         ESP_LOGI("debug_flag", "before publish to AWS");
         while(!AreMeasurementsReady)
         {
            vTaskDelay(pdMS_TO_TICKS(500));
         }
         // publish_reading(&AllSensorsReadings, ErrId);
         ESP_LOGI("debug_flag", "after publish readings func");
#ifdef DEBUG
         ESP_LOGI("sensor_app", "====================================");
         ESP_LOGI("sensor_app", "soil moisture: %u ", AllSensorsReadings.soil_moisture);
         ESP_LOGI("sensor_app", "humidity: %f , temperature: %f ", AllSensorsReadings.humidity, AllSensorsReadings.temperature);
         ESP_LOGI("sensor_app", "illuminance: %lu ", AllSensorsReadings.illuminance);
         ESP_LOGI("sensor_app", "Is WiFi connected: %s", wifi_connected == 1 ? "true" : "false");
         ESP_LOGI("sensor_app", "Is AWS connected: %s", mqtt_connected == 1 ? "true" : "false");
         ESP_LOGI("sensor_app", "====================================");
#endif
      }
      ESP_LOGI("debug_flag", "before go to sleep");
      // GoToSleep(ONE_MINS_IN_US * 3);
      vTaskDelay(pdMS_TO_TICKS(10000));
      ESP_LOGI("debug_flag", "after sleep");
   }
}
