/*!
*******************************************************
* \file:
* \date:
* \author:
* \brief:
********************************************************/
#ifndef USERCONFIG_H_
#define USERCONFIG_H_
//======================================[INCLUDES]======================================//

//=================================[MACROS AND DEFINES]=================================//
#define MEAS_EVENTS_AMOUNT        2
#define WAKEUP_EVENT_HOUR_1       8
#define WAKEUP_EVENT_MINS_1       0
#define WAKEUP_EVENT_HOUR_2       19
#define WAKEUP_EVENT_MINS_2       15

#define MAX_NO_CONNECTION_ATTEMPS 3
//======================================[TYPEDEFS]======================================//

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
void RegisterAllCallbacks(void);

#endif