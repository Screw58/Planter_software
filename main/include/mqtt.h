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
#define MAX_RETRY       10
#define TEST_TOPIC      "/home/sensors"
#define MQTT_BROKER_URL "mqtt://192.168.0.100"
#define TAG_MQTT        "mqtt_file"
//======================================[TYPEDEFS]======================================//

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
void Mqtt_Init(void);
void publish_reading(const AllSensorsReadings_t *AllSensorsReadings, const ErrorId_t err_id);
#endif