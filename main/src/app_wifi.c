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
#include "secret_data.h"

//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//

#define MAX_RETRY 10

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
static int cnt_retry = 0;
static wifi_conn_params_f wifi_params;


//==================================[GLOBAL VARIABLES]==================================//
bool wifi_connected = false;

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
      if(cnt_retry < MAX_RETRY)
      {
         esp_wifi_connect();
         cnt_retry++;
      }
      else
      {
         wifi_params.on_failed();
      }
   }
   else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
   {
      cnt_retry = 0;
      wifi_params.on_connected();
   }
}

//==================================[GLOBAL FUNCTIONS]==================================//

void AppWifiConnect(wifi_conn_params_f wifi_callback)
{
   wifi_params = wifi_callback;

   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
   wifi_config_t wifi_config = {
	 .sta =
	     {
		   .ssid = WIFI_SSID,
		   .password = WIFI_PASS,
		   .threshold.authmode = WIFI_AUTH_WPA2_PSK,
		   .pmf_cfg = {.capable = true, .required = false},
	     },
   };

   if(nvs_flash_init() != ESP_OK)
   {
      nvs_flash_erase();
      nvs_flash_init();
   }
   esp_event_loop_create_default();
   esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiEventHandler, NULL);
   esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WifiEventHandler, NULL);

   esp_netif_init();

   esp_netif_create_default_wifi_sta();

   esp_wifi_init(&cfg);
   esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
   esp_wifi_set_mode(WIFI_MODE_STA);
   esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
   esp_wifi_start();
}
