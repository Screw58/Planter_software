/*!
*******************************************************
* \file:
* \date:
* \author:
* \brief:
********************************************************/

#ifndef MQTT_H_
#define MQTT_H_

//======================================[INCLUDES]======================================//
#include "err_handler.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "sensors.h"


//=================================[MACROS AND DEFINES]=================================//
#define DEVICE_ID             "dev_1"
#define SENSOR_READINGS_TOPIC "/home/sensors_readings"
#define SENSOR_ERRORS_TOPIC   "/home/sensors_errors"
#define MQTT_BROKER_URL       "mqtt://192.168.0.100"
#define TAG_MQTT              "mqtt_file"
//======================================[TYPEDEFS]======================================//
typedef void (*MqttConnected_f)(void);
typedef void (*MqttError_f)(void);
typedef void (*MqttDataRcv_f)(void);

typedef struct {
   MqttConnected_f MqttConnectedCbk;
   MqttError_f MqttErrorCbk;
   MqttDataRcv_f MqttDataRcvCbk;
} Mqtt_Callback_t;

typedef enum
{
   MQTT_INIT,
   MQTT_CONNECTED,
   MQTT_ERROR,
   MQTT_DATA_RECEIVED,
} MqttStatus_t;
//=================================[EXPORTED VARIABLES]=================================//
// extern esp_mqtt_client_handle_t mqtt_client;
//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
/*!
 * \brief:
 * \details:
 */
void Mqtt_Connect(void);

/*!
 * \brief:
 * \details:
 */
void Mqtt_Publish_Readings(const AllSensorsReadings_t *const AllSensorsReadings);

/*!
 * \brief:
 * \details:
 */
MqttStatus_t MqttGetStatus(void);

/*!
 * \brief:
 * \details:
 */
void Mqtt_Disconnect(void);

/*!
 * \brief:
 * \details:
 */
void MqttRegisterCallbacks(Mqtt_Callback_t UserCallbacks);

/*!
 * \brief:
 * \details:
 */
void Mqtt_Publish_Errors(const AllSensorsErrors_t *const AllSensorsErrors);

#endif
