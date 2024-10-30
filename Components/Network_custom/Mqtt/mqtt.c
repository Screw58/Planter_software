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

//==================================[GLOBAL VARIABLES]==================================//
esp_mqtt_client_handle_t mqtt_client;
//=============================[LOCAL FUNCTION PROTOTYPES]==============================//
/*!
 * \brief:
 * \details:
 */
static void Mqtt_EventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

//==================================[LOCAL FUNCTIONS]===================================//
static void Mqtt_EventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
   esp_mqtt_event_handle_t event = event_data;

   switch((esp_mqtt_event_id_t)event_id)
   {
      case MQTT_EVENT_CONNECTED:
         ESP_LOGI(TAG_MQTT, "mqtt broker connected");
         mqtt_connected = true;
         ESP_LOGI("mqtt", "MQTT CONNECTED!!");
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

//==================================[GLOBAL FUNCTIONS]==================================//
void Mqtt_Connect(void)
{
   const esp_mqtt_client_config_t mqtt_cfg = {
      .broker.address.uri = MQTT_BROKER_URL,
   };
   mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
   esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, Mqtt_EventHandler, mqtt_client);
   esp_mqtt_client_start(mqtt_client);
}

void Mqtt_Disconnect(void)
{
   esp_mqtt_client_stop(mqtt_client);
}

void Mqtt_Publish_Readings(const AllSensorsReadings_t *const AllSensorsReadings, const ErrorId_t err_id)
{
   char buffer[150] = { 0 };

   // memset((void *)&message, 0, sizeof(message));

   sprintf(
         buffer,
         "{ \"soilhumidity\" : %d , \"temperature\" : %.2f , \"humidity\" : %.2f , \"illuminance\" : %ld , \"errors\" : %d, \"device_id\" "
         ": \"sensor_3\"}",
         AllSensorsReadings->soil_moisture,
         AllSensorsReadings->temperature,
         AllSensorsReadings->humidity,
         AllSensorsReadings->illuminance,
         err_id);

   if(mqtt_client != NULL)
   {
      esp_mqtt_client_publish(mqtt_client, TEST_TOPIC, buffer, 0, 2, 0);
   }
}
