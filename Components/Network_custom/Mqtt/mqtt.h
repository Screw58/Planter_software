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
#define TEST_TOPIC      "/home/sensors"
#define MQTT_BROKER_URL "mqtt://192.168.0.100"
#define TAG_MQTT        "mqtt_file"
//======================================[TYPEDEFS]======================================//

//=================================[EXPORTED VARIABLES]=================================//
extern esp_mqtt_client_handle_t mqtt_client;
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
void Mqtt_Publish_Readings(const AllSensorsReadings_t *const AllSensorsReadings, const ErrorId_t err_id);

#endif