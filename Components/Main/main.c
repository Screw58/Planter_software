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

#define DEBUG
//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
static volatile AllSensorsReadings_t AllSensorsReadings = { 0 };

//==================================[GLOBAL VARIABLES]==================================//
wifi_conn_params_f wifi_callback;
bool wifi_connected = false;
bool mqtt_connected = false;
bool AreMeasurementsReady = false;

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
 * \brief:
 * \details:
 */
static void PrintDebugInfo(void);

//==================================[LOCAL FUNCTIONS]===================================//

static void WifiConnectedHandler(void)
{
   ESP_LOGE(TAG_MAIN, "Wifi succesfully connected!");
   wifi_connected = true;

   Mqtt_Connect();
}

static void WifiFailedHandler(void)
{
   ESP_LOGE(TAG_MAIN, "Wifi connection failed");
}


static void PrintDebugInfo(void)
{
   ESP_LOGI("sensor_app", "====================================");
   ESP_LOGI("sensor_app", "soil moisture: %u ", AllSensorsReadings.soil_moisture);
   ESP_LOGI("sensor_app", "humidity: %f , temperature: %f ", AllSensorsReadings.humidity, AllSensorsReadings.temperature);
   ESP_LOGI("sensor_app", "illuminance: %lu ", AllSensorsReadings.illuminance);
   ESP_LOGI("sensor_app", "Is WiFi connected: %s", wifi_connected == 1 ? "true" : "false");
   ESP_LOGI("sensor_app", "Is Mqtt connected: %s", mqtt_connected == 1 ? "true" : "false");
   ESP_LOGI("sensor_app", "====================================");
}

void HardwareInit(void)
{
   /* Peripherals initialization */
   ADC_Init();
   I2C_Init();
   LED_Init();
   Turn_LED_On(LED_COLOUR_ORANGE);
}


//==================================[GLOBAL FUNCTIONS]==================================//

void app_main()
{
   ErrorId_t ErrId = 0;

   wifi_callback.on_connected = WifiConnectedHandler;
   wifi_callback.on_failed = WifiFailedHandler;
   CheckWakeUpReason();

   HardwareInit();
   SensorsInit();

   /* Time for aht15 startup */
   vTaskDelay(pdMS_TO_TICKS(60));

   mqtt_connected = false;
   wifi_connected = false;

   AppWifiConnect(wifi_callback);
   vTaskDelay(pdMS_TO_TICKS(2000));

   while(1)
   {
      /* Wait until the Mqtt client connects to the server */
      ESP_LOGI("debug_flag", "before aws connected");
      while(!mqtt_connected)
      {
         vTaskDelay(pdMS_TO_TICKS(500));
      }
      ESP_LOGI("debug_flag", "before checking conditions");
      if((wifi_connected == true) && (mqtt_connected == true))
      {
         Turn_LED_On(LED_COLOUR_CYAN);
         // AreMeasurementsReady = false;
         TakeSensorMeasurements(&AllSensorsReadings);
         ErrId = CheckErrors();
         CheckBatteryVoltage();
         // PrepareDataToSendAWS(&soilsensor1, &aht15sensor1, &lightsensor1);
         ESP_LOGI("debug_flag", "before publish to AWS");
         // while(!AreMeasurementsReady)
         // {
         //    vTaskDelay(pdMS_TO_TICKS(500));
         // }
         // Mqtt_Publish_Readings(&AllSensorsReadings, ErrId);
         ESP_LOGI("debug_flag", "after publish readings func");
         Turn_LED_Off();
         vTaskDelay(pdMS_TO_TICKS(3000));
         Blink_LED(LED_COLOUR_PINK, 8, 200);
         vTaskDelay(pdMS_TO_TICKS(6000));

#ifdef DEBUG
         PrintDebugInfo();
#endif
      }
      ESP_LOGI("debug_flag", "before go to sleep");
      vTaskDelay(pdMS_TO_TICKS(7000));
      GoToSleep();
      // vTaskDelay(pdMS_TO_TICKS(5000));
      ESP_LOGI("debug_flag", "after sleep");
   }
}
