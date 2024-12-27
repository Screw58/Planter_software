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
#include "UserConfig.h"
//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//
#define TAG_MAIN "main_app"

#define DEBUG
//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
static volatile AllSensorsReadings_t AllSensorsReadings = { 0 };

//==================================[GLOBAL VARIABLES]==================================//
WifiStatus_t WifiStatus;
MqttStatus_t MqttStatus;
bool AreMeasurementsReady = false;

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//

/*!
 * \brief:
 * \details:
 */
static void PrintDebugInfo(void);

//==================================[LOCAL FUNCTIONS]===================================//

static void PrintDebugInfo(void)
{
   ESP_LOGI("sensor_app", "====================================");
   ESP_LOGI("sensor_app", "soil moisture: %u ", AllSensorsReadings.soil_moisture);
   ESP_LOGI("sensor_app", "humidity: %f , temperature: %f ", AllSensorsReadings.humidity, AllSensorsReadings.temperature);
   ESP_LOGI("sensor_app", "illuminance: %lu ", AllSensorsReadings.illuminance);
   // ESP_LOGI("sensor_app", "Is WiFi connected: %s", wifi_connected == 1 ? "true" : "false");
   // ESP_LOGI("sensor_app", "Is Mqtt connected: %s", mqtt_connected == 1 ? "true" : "false");
   ESP_LOGI("sensor_app", "====================================");
}

void PeripInit(void)
{
   /* Peripherals initialization */
   ADC_Init();
   I2C_Init();
   LED_Init();
   // Turn_LED_On(LED_COLOUR_ORANGE);
}


//==================================[GLOBAL FUNCTIONS]==================================//

void app_main()
{
   ErrorId_t ErrId = 0;

   CheckWakeUpReason();

   PeripInit();
   // PeripInitStatus = PeripInit();
   //  if(PeripInitStatus == INIT_OK)
   //{
   // SensorsInitStatus = SensorsInit();
   SensorsInit();
   //}
   RegisterAllCallbacks();

   WifiConnect();

   /* Time for aht15 startup */
   vTaskDelay(pdMS_TO_TICKS(5000));


   // AppWifiConnect(wifi_callback);
   // vTaskDelay(pdMS_TO_TICKS(2000));

   while(1)
   {
      /* Wait until the Mqtt client connects to the server */
      ESP_LOGI("debug_flag", "before aws connected");

      WifiStatus = GetWifiStatus();
      MqttStatus = MqttGetStatus();

      if(WifiStatus == WIFI_CONNECTED && MqttStatus == MQTT_CONNECTED)
      {
         // if(SensorsInitStatus == INIT_OK)
         // {
         TakeSensorMeasurements(&AllSensorsReadings);
         ErrId = CheckErrors();
         CheckBatteryVoltage();
         //}
         // Mqtt_Publish_Readings(&AllSensorsReadings, ErrId);
         vTaskDelay(pdMS_TO_TICKS(7000));
         GoToSleep();
      }
      else
      {
         /*wait and try to reconnect */
         vTaskDelay(pdMS_TO_TICKS(500));
      }
      // while(!mqtt_connected)
      // {
      //    vTaskDelay(pdMS_TO_TICKS(500));
      // }

      // Turn_LED_On(LED_COLOUR_CYAN);
      // AreMeasurementsReady = false;


      // PrepareDataToSendAWS(&soilsensor1, &aht15sensor1, &lightsensor1);
      // ESP_LOGI("debug_flag", "before publish to AWS");
      // while(!AreMeasurementsReady)
      // {
      //    vTaskDelay(pdMS_TO_TICKS(500));


      // #ifdef DEBUG
      //       PrintDebugInfo();
      // #endif
   }
}
