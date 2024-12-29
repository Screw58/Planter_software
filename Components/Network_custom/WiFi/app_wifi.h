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
typedef void (*WifiReconnecting_f)(uint8_t);
typedef void (*WifiFailed_f)(void);
typedef void (*WifiConnected_f)(void);

typedef struct {
   WifiReconnecting_f WifiOnReconnecting;
   WifiFailed_f WifiOnfailed;
   WifiConnected_f WifiOnConnected;
} Wifi_Callback_t;

typedef enum
{
   WIFI_INIT,
   WIFI_DISCONNECTED,
   WIFI_RECONNECTING,
   WIFI_CONNECTED,
} WifiStatus_t;
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
void WifiConnect(void);

void WifiRegisterCallbacks(Wifi_Callback_t Wifi_StatesCallbacks);
HourAndMins_t SNTP_ReadTime(void);
// static void handle_mqtt_events(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
// void publish_reading(const AllSensorsReadings_t *AllSensorsReadings, const ErrorId_t err_id);
void Wifi_Stop(void);
WifiStatus_t GetWifiStatus(void);

#endif