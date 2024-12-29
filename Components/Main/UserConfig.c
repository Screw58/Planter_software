/*!
******************************************************************
* \file:UserConfig.c
* \date: 29.10.2024
* \author: Jacek Twardoch
* \brief: File for configuration parameters provided by user
******************************************************************/

//======================================[INCLUDES]======================================//
#include "UserConfig.h"
#include "app_wifi.h"
#include "esp_log.h"
#include "led.h"
#include "mqtt.h"


//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//

//==================================[GLOBAL VARIABLES]==================================//

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//
void OnWifiConnected(void);
void OnWifiFailed(void);
void OnWifiReconnecting(uint8_t ReconnCnt);
void RegisterWifiCallbacks(void);
void OnMqttConnected(void);
void OnMqttError(void);
void OnMqttDataRcv(void);
void RegisterMqttCallbacks(void);
//==================================[LOCAL FUNCTIONS]===================================//

/* WIFI CALLBACKS */
void OnWifiConnected(void)
{
   Turn_LED_On(LED_COLOUR_GREEN);
   Mqtt_Connect();
   ESP_LOGI("WiFi", "WiFi CONNECTED!");
}

void OnWifiFailed(void)
{
   Turn_LED_On(LED_COLOUR_RED);
   // Maybe beeping ?
   ESP_LOGI("WiFi", "Wifi Failed");
   //!< TODO - Set global error
   GoToSleep();
}

void OnWifiReconnecting(uint8_t ReconnCnt)
{
   Blink_LED(LED_COLOUR_ORANGE, 15, 500);
   ESP_LOGI("User", "Wifi reconnecting... Attempt: %u", ReconnCnt);
}

void RegisterWifiCallbacks(void)
{
   Wifi_Callback_t callbacks = {
      .WifiOnConnected = OnWifiConnected,
      .WifiOnfailed = OnWifiFailed,
      .WifiOnReconnecting = OnWifiReconnecting,
   };
   /* Register created callbacks into WiFi module */
   WifiRegisterCallbacks(callbacks);
}

/* MQTT CALLBACKS */
void OnMqttConnected(void)
{
   ESP_LOGI("mqtt", "MQTT CONNECTED!!");
   Blink_LED(LED_COLOUR_CYAN, 2, 300);
   // Turn_LED_On(LED_COLOUR_CYAN);
}
void OnMqttError(void)
{
   Turn_LED_On(LED_COLOUR_RED);
   // ESP_LOGE(TAG_MQTT, "errtype: %d", event->error_handle->error_type);
   // Maybe beeping ?
   //!< TODO - Set global error
   GoToSleep();
}
void OnMqttDataRcv(void)
{
   ESP_LOGI(TAG_MQTT, "mqtt receive message");
}

void RegisterMqttCallbacks(void)
{
   Mqtt_Callback_t callbacks = {
      .MqttConnectedCbk = OnMqttConnected,
      .MqttErrorCbk = OnMqttError,
      .MqttDataRcvCbk = OnMqttDataRcv,
   };
   /* Register created callbacks into WiFi module */
   MqttRegisterCallbacks(callbacks);
}
//==================================[GLOBAL FUNCTIONS]==================================//
void RegisterAllCallbacks(void)
{
   RegisterWifiCallbacks();
   RegisterMqttCallbacks();
}