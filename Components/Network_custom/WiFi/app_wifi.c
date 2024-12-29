/*!
******************************************************************
* \file: app_wifi.c
* \date: 09.10.23
* \author: Jacek Twardoch
* \brief: This file contains definitions of two functions - first one
          for connecting to wifi and the second one for handling with
          wifi events
******************************************************************/

//======================================[INCLUDES]======================================//
#include "app_wifi.h"
#include "err_handler.h"
#include "esp_netif_sntp.h"

// #include "esp_sntp.h"
// #include "mqtt_client.h"
#include "secret_data.h"
#include "UserConfig.h"
//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//

#define MAX_RETRY_NUM MAX_NO_CONNECTION_ATTEMPS

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
static uint8_t ConnectionRetry_Cnt = 0;
static Wifi_Callback_t Wifi_Callbacks;

static struct timeval time_in_sec;
static WifiStatus_t Wifi_status;

//==================================[GLOBAL VARIABLES]==================================//
struct tm time_and_date = { 0 };
struct timeval *tv = { 0 };
struct tm *time_and_date_ptr;

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//
/*!
 * \brief: Handler for some events occurence during wifi connection process
 * \details: Basic functions:
 *             -If esp wifi driver is started succesfully then try to wifi connect
 *             -If some issues occur during wifi connection try to connect again
 *             -If the esp will be succesfully connected to wifi, then start aws connection process
 */
static void WifiEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

//==================================[LOCAL FUNCTIONS]===================================//

static void WifiEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
   if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
   {
      esp_wifi_connect();
   }
   else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
   {
      if(ConnectionRetry_Cnt < MAX_RETRY_NUM)
      {
         Wifi_status = WIFI_RECONNECTING;
         esp_wifi_connect();
         ConnectionRetry_Cnt++;
         Wifi_Callbacks.WifiOnReconnecting(ConnectionRetry_Cnt);
      }
      else
      {
         Wifi_status = WIFI_DISCONNECTED;
         Wifi_Callbacks.WifiOnfailed();
      }
   }
   else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
   {
      ConnectionRetry_Cnt = 0;
      Wifi_status = WIFI_CONNECTED;
      Wifi_Callbacks.WifiOnConnected();
   }
}

//==================================[GLOBAL FUNCTIONS]==================================//

void WifiConnect(void)
{

   wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
   esp_event_handler_instance_t instance_any_id;
   esp_event_handler_instance_t instance_got_ip;
   wifi_config_t wifi_cfg = { 
      .sta = { 
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
   },
};

   Wifi_status = WIFI_INIT;

   if(nvs_flash_init() != ESP_OK)
   {
      nvs_flash_erase();
      nvs_flash_init();
   }
   ESP_ERROR_CHECK(esp_netif_init());
   ESP_ERROR_CHECK(esp_event_loop_create_default());
   esp_netif_create_default_wifi_sta();

   ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));


   ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiEventHandler, NULL, &instance_any_id));
   ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WifiEventHandler, NULL, &instance_got_ip));

   ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
   ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));


   ESP_ERROR_CHECK(esp_wifi_start());
}

// Zmienic nazwe i wolac z errorhandlera
void WifiRegisterCallbacks(Wifi_Callback_t Wifi_StatesCallbacks)
{
   Wifi_Callbacks = Wifi_StatesCallbacks;
   // WifiConnect();
}

HourAndMins_t SNTP_ReadTime(void)
{
   time_t now;
   HourAndMins_t RetTime = { 0 };
   esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(
         1,
         ESP_SNTP_SERVER_LIST("pool.ntp.org"));  // !< TODO zmienić ten default na normalny init    "time.windows.com"
   // config.sync_cb = time_sync_callback;
   esp_netif_sntp_init(&config);
   esp_netif_sntp_start();
   esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000));
   time(&now);
   setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
   tzset();
   time_and_date_ptr = localtime(&now);
   if(time_and_date_ptr == NULL)
   {
      ESP_LOGE("time", "NULL pointer was returned from localtime() function! ");
   }
   else
   {
      RetTime.hour = time_and_date_ptr->tm_hour;
      RetTime.mins = time_and_date_ptr->tm_min;

      ESP_LOGI("time", "time printed from function");
      ESP_LOGI("time", "time_second: %d ", time_and_date_ptr->tm_sec);
      ESP_LOGI("time", "time_mins: %d ", time_and_date_ptr->tm_min);
      ESP_LOGI("time", "time_hours: %d ", time_and_date_ptr->tm_hour);
      ESP_LOGI("time", "tm_mday: %d ", time_and_date_ptr->tm_mday);
      ESP_LOGI("time", "tm_year: %d ", time_and_date_ptr->tm_year);
      ESP_LOGI("time", "tm_wday: %d \n", time_and_date_ptr->tm_wday);
   }

   return RetTime;
}

void Wifi_Stop(void)
{
   esp_wifi_stop();
}

WifiStatus_t GetWifiStatus(void)
{
   return Wifi_status;
}
//!< TODO - zmienic nazwe funkcji ktora handluje callbacki i dodac je w errorhandlerze
//