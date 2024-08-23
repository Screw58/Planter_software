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
// #include "mqtt_client.h"
// #include "secret_data.h"

//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//

#define MAX_RETRY 10


#define WIFI_SSID "SIL_117"
#define WIFI_PASS "wyprosi60"
//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
static int cnt_retry = 0;
static wifi_conn_params_f wifi_params;

// static esp_mqtt_client_handle_t mqtt_client = NULL;
// static void handle_mqtt_events (void *handler_args, esp_event_base_t base, int32_t event_id, void * event_data);
//==================================[GLOBAL VARIABLES]==================================//
// bool wifi_connected = false;
// bool mqtt_connected = false;
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


void AppWifiConnect(wifi_conn_params_f wifi_callback)
{
   wifi_params = wifi_callback;
   WifiConnect();
}
//    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//    wifi_config_t wifi_config = {
// 	 .sta =
// 	     {
// 		   .ssid = WIFI_SSID,
// 		   .password = WIFI_PASS,
// 		   .threshold.authmode = WIFI_AUTH_WPA2_PSK,
// 		   .pmf_cfg = {.capable = true, .required = false},
// 	     },
//    };

//    if(nvs_flash_init() != ESP_OK)
//    {
//       nvs_flash_erase();
//       nvs_flash_init();
//    }
//    esp_event_loop_create_default();
//    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiEventHandler, NULL);
//    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WifiEventHandler, NULL);

//    esp_netif_init();

//    esp_netif_create_default_wifi_sta();

//    esp_wifi_init(&cfg);
//    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
//    esp_wifi_set_mode(WIFI_MODE_STA);
//    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
//    esp_wifi_start();
// }


// void PublishReadingsToAws(const AllSensorsReadings_t *AllSensorsReadings, const ErrorId_t err_id)
// {
//    char buffer[150];
//    esp_mqtt_client_config_t mqtt_config = {
//       .broker.address.uri = MQTT_BROKER_URL;
//    }
//    // IoT_Error_t result;
//    // IoT_Publish_Message_Params message;
//    ESP_LOGI("debug_flag", "before aws-mqtt-yield");
//    // result = aws_iot_mqtt_yield(&aws_client, 500);
//    // if(result != SUCCESS && result != NETWORK_RECONNECTED)
//    // {
//    //    ESP_LOGI("debug_flag", "in if condition ERROR!");
//    //    result = aws_iot_mqtt_yield(&aws_client, 500);
//    //    ESP_LOGI("debug_flag", "result of the second connection: %d", result);
//    //    // return;
//    // }

//    memset((void *)&message, 0, sizeof(message));
//    // itoa(temp, buffer, 10);
//    sprintf(buffer,
//            "{ \"soilhumidity\" : %d , \"temperature\" : %.2f , \"humidity\" : %.2f , \"illuminance\" : %d , \"errors\" : %d,
//            \"device_id\" "
//            ": \"sensor_3\"}",
//            AllSensorsReadings->soil_moisture,
//            AllSensorsReadings->temperature,
//            AllSensorsReadings->humidity,
//            (uint16_t)AllSensorsReadings->illuminance,
//            err_id);

//    message.qos = QOS0;
//    message.payload = (void *)buffer;
//    message.payloadLen = strlen(buffer);
//    // aws_iot_mqtt_publish(&aws_client, TEMP_TOPIC, strlen(TEMP_TOPIC), &message);
//    ESP_LOGI("debug_flag", "before mqtt publish");
//    aws_iot_mqtt_publish(&aws_client, test_topic, strlen(test_topic), &message);
// }

// void WifiConnectedHandler(void)
// {
//    ESP_LOGE(TAG, "Wifi succesfully connected!");
//    wifi_connected = true;
//    // ConnectAwsMqtt();
//    esp_mqtt_client_config_t mqtt_cfg = {
//       .uri = MQTT_BROKER_URL,
//    };
//    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
//    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, handle_mqtt_events, NULL);
//    esp_mqtt_client_start(mqtt_client);
//    // apptemp_init(publish_reading)
// }


// static void handle_mqtt_events(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
// {
//    esp_mqtt_event_handle_t event = event_data;
//    switch((esp_mqtt_event_id_t)event_id)
//    {
//       case MQTT_EVENT_CONNECTED:
//          ESP_LOGI(TAG, "mqtt broker connected");
//          mqtt_connected = true;
//          break;
//       case MQTT_EVENT_DATA:
//          ESP_LOGI(TAG, "mqtt receive message");
//          break;
//       case MQTT_EVENT_ERROR:
//          ESP_LOGE(TAG, "errtype: %d", event->error_handle->error_type);
//          break;
//       default:
//          ESP_LOGI(TAG, "event: %d", event_id);
//          break;
//    }
// }
