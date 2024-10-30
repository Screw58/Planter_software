/*!
 *******************************************************
 * \file: app_wifi
 * \date: 15.10
 * \author: Jacek Twardoch
 * \brief: This file contains necessary type defines for
           wifi connection callbacks and global function
           prototype
 ********************************************************/

#ifndef WIFI_H_
#define WIFI_H_

//======================================[INCLUDES]======================================//
#include "err_handler.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include "sensors.h"

#include "esp_wifi.h"
#include "McuState.h"
//=================================[MACROS AND DEFINES]=================================//

//======================================[TYPEDEFS]======================================//
/* callback functions prototypes for wifi connection status*/
typedef void (*OnConnected_f)(void);
typedef void (*OnFailed_f)(void);

typedef struct {
   OnConnected_f on_connected;
   OnFailed_f on_failed;
} wifi_conn_params_f;


//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
/*!
 * \brief: This function tries to connect esp to wifi
 * \details: Following steps are performed:
 *             -Initialize nvs
 *             -Create default system event
 *             -Register function to handle with events
 *             -Set wifi_configure structure
 *             -Initialize netif
 *             -Initialize wifi driver
 *             -Set mode and start wifi
 */
void AppWifiConnect(wifi_conn_params_f wifi_callback);
HourAndMins_t SNTP_ReadTime(void);
// static void handle_mqtt_events(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void publish_reading(const AllSensorsReadings_t *AllSensorsReadings, const ErrorId_t err_id);
void Wifi_Stop(void);
#endif