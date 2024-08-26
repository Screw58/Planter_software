/*!
*******************************************************
* \file: main.h
* \date: 09.10.23
* \author: Jacek Twardoch
* \brief: Necessary headers and general defines with macros
********************************************************/

#ifndef MAIN_H_
#define MAIN_H_
//======================================[INCLUDES]======================================//
/* Standard libraries */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Espressif libraries */
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* User libraries */
#include "app_wifi.h"
#include "battery.h"
#include "err_handler.h"
#include "mqtt.h"
#include "sensors.h"


//=================================[MACROS AND DEFINES]=================================//
#define ERROR_PRESENT     1
#define ERROR_NOT_PRESENT 0

#define WAIT_IN_MS(x)     (vTaskDelay(pdMS_TO_TICKS(x)))
//======================================[TYPEDEFS]======================================//

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
// void HardwareInit(void);
#endif