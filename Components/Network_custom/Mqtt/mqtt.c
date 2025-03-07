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

//=============================[PRIVATE MACROS AND DEFINES]=============================//

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//

//==================================[GLOBAL VARIABLES]==================================//
esp_mqtt_client_handle_t mqtt_client;
Mqtt_Callback_t MqttCallbacks;
MqttStatus_t Mqtt_status;
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
         Mqtt_status = MQTT_CONNECTED;
         MqttCallbacks.MqttConnectedCbk();
         break;
      case MQTT_EVENT_DATA:
         Mqtt_status = MQTT_DATA_RECEIVED;
         MqttCallbacks.MqttDataRcvCbk();
         break;
      case MQTT_EVENT_ERROR:
         Mqtt_status = MQTT_ERROR;
         MqttCallbacks.MqttErrorCbk();
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

   Mqtt_status = MQTT_INIT;
   mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
   esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, Mqtt_EventHandler, mqtt_client);
   esp_mqtt_client_start(mqtt_client);
}

void Mqtt_Publish_Readings(const AllSensorsReadings_t *const AllSensorsReadings)
{
   char buffer[150] = { 0 };

   // memset((void *)&message, 0, sizeof(message));

   sprintf(buffer,
           "{ \"soilhumidity\" : %d , \"temperature\" : %.2f , \"humidity\" : %.2f , \"illuminance\" : %ld , \"device_id\" "
           ": \"" DEVICE_ID "\"}",
           AllSensorsReadings->soil_moisture,
           AllSensorsReadings->temperature,
           AllSensorsReadings->humidity,
           AllSensorsReadings->illuminance);

   if(mqtt_client != NULL)
   {
      esp_mqtt_client_publish(mqtt_client, SENSOR_READINGS_TOPIC, buffer, 0, 2, 0);
   }
}

void Mqtt_Publish_Errors(const AllSensorsErrors_t *const AllSensorsErrors)
{
   char buffer[100] = { 0 };

   // memset((void *)&message, 0, sizeof(message));

   sprintf(buffer,
           "{ \"soilsenserr\" : %d , \"aht15senserr\" : %d , \"bh1750senserr\" : %d , \"device_id\" "
           ": \"" DEVICE_ID "\"}",
           AllSensorsErrors->SoilSensErr,
           AllSensorsErrors->Aht15SensErr,
           AllSensorsErrors->BH1750SensErr);

   if(mqtt_client != NULL)
   {
      esp_mqtt_client_publish(mqtt_client, SENSOR_ERRORS_TOPIC, buffer, 0, 2, 0);
   }
}

void MqttRegisterCallbacks(Mqtt_Callback_t UserCallbacks)
{
   MqttCallbacks = UserCallbacks;
}

void Mqtt_Disconnect(void)
{
   esp_mqtt_client_stop(mqtt_client);
}

MqttStatus_t MqttGetStatus(void)
{
   return Mqtt_status;
}