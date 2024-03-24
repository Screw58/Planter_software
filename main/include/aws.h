/*!
*******************************************************
* \file: aws.h
* \date: 20.11.23
* \author: Jacek Twardoch
* \brief: This file contains function prototypes for
*        handling with aws client
********************************************************/

#ifndef AWS_H_
#define AWS_H_

//======================================[INCLUDES]======================================//
#include "esp_log.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_version.h"
#include "err_handler.h"

//=================================[MACROS AND DEFINES]=================================//

//======================================[TYPEDEFS]======================================//

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//

/*!
 * \brief: Function which will be executed after aws disconnection
 */
void disconnected_handler(AWS_IoT_Client *pClient, void *data);

/*!
 * \brief: It initializes aws client and then calls aws_iot_mqtt_connect() function which
 *        sends the MQTT CONNECT packet to the server, which establishes an MQTT session
 */
void ConnectAwsMqtt(void);

/*!
 * \brief: This function sends data to aws client via MQTT protocol
 * \details: First it glues all data into the json payload in buffer, then it sends
 *          buffer content to aws client
 */
void PublishReadingsToAws(const AllSensorsReadings_t *AllSensorsReadings, const ErrorId_t err_id);

/*!
 * \brief: This function calls aws_iot_mqtt_disconnect() function that sends
 *        the MQTT DISCONNECT packet, which closes the MQTT session between the client
 *        and server
 */
void AwsDisconnect(void);

#endif
