/*!
******************************************************************
* \file:
* \date:
* \author:
* \brief:
******************************************************************/

//======================================[INCLUDES]======================================//
#include "mqtt.h"
//==================================[EXTERN VARIABLES]==================================//
extern bool mqtt_connected;
//=============================[PRIVATE MACROS AND DEFINES]=============================//

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
static esp_mqtt_client_handle_t mqtt_client;
//==================================[GLOBAL VARIABLES]==================================//

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//
static void handle_mqtt_events(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
//==================================[LOCAL FUNCTIONS]===================================//

//==================================[GLOBAL FUNCTIONS]==================================//

void Mqtt_Init(void)
{

   const esp_mqtt_client_config_t mqtt_cfg = {
      .broker.address.uri = MQTT_BROKER_URL,
   };
   mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
   esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, handle_mqtt_events, mqtt_client);
   esp_mqtt_client_start(mqtt_client);
}


static void handle_mqtt_events(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
   esp_mqtt_event_handle_t event = event_data;
   switch((esp_mqtt_event_id_t)event_id)
   {
      case MQTT_EVENT_CONNECTED:
         ESP_LOGI(TAG_MQTT, "mqtt broker connected");
         mqtt_connected = true;
         break;
      case MQTT_EVENT_DATA:
         ESP_LOGI(TAG_MQTT, "mqtt receive message");
         break;
      case MQTT_EVENT_ERROR:
         ESP_LOGE(TAG_MQTT, "errtype: %d", event->error_handle->error_type);
         break;
      default:
         ESP_LOGI(TAG_MQTT, "event: %ld", event_id);
         break;
   }
}

void publish_reading(const AllSensorsReadings_t *AllSensorsReadings, const ErrorId_t err_id)
{
   char buffer[150];

   // memset((void *)&message, 0, sizeof(message));

   sprintf(buffer,
           "{ \"soilhumidity\" : %d , \"temperature\" : %.2f , \"humidity\" : %.2f , \"illuminance\" : %d , \"errors\" : %d, \"device_id\" "
           ": \"sensor_3\"}",
           AllSensorsReadings->soil_moisture,
           AllSensorsReadings->temperature,
           AllSensorsReadings->humidity,
           (uint16_t)AllSensorsReadings->illuminance,
           err_id);
   if(mqtt_client != NULL)
   {
      esp_mqtt_client_publish(mqtt_client, TEST_TOPIC, buffer, 0, 2, 0);
   }
}
