/*!
*******************************************************
* \file: McuState.h
* \date:
* \author:
* \brief:
********************************************************/
#ifndef MCU_STATE_H_
#define MCU_STATE_H_
//======================================[INCLUDES]======================================//
#include "esp_sleep.h"
//=================================[MACROS AND DEFINES]=================================//
#define WAKEUP_BUTTON_PIN GPIO_NUM_4
//======================================[TYPEDEFS]======================================//
typedef struct {
   int hour;
   int mins;
} HourAndMins_t;


//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
/*!
 * \brief: This function prepares and puts MCU to sleep
 * \details: Before sleep aws client is disconnected and wifi module is turned off
 */
void GoToSleep(void);

void CheckWakeUpReason(void);

#endif
