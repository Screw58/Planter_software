/*!
*******************************************************
* \file: ErrHandler.h
* \date: 20.11.23
* \author: Jacek Twardoch
* \brief:
********************************************************/
#ifndef ERRHANDLER_H_
#define ERRHANDLER_H_


//======================================[INCLUDES]======================================//
#include "adc.h"
#include "i2c.h"
#include "sensors.h"

#include "app_wifi.h"

//=================================[MACROS AND DEFINES]=================================//

//======================================[TYPEDEFS]======================================//
typedef enum
{
   SoilSensErrID = 1,
   ERROR_ID_POS_MIN = SoilSensErrID,
   Aht15BusyErrID = 2,
   Aht15CalibErrID = 3,
   I2cBusErrID = 4,
   TooManyErrID = 5,
   ERROR_ID_POS_MAX = TooManyErrID
} ErrorId_t;

typedef struct {
   uint16_t SoilSensErr  : 1;
   uint16_t Aht15BusyErr : 1;
   uint16_t Aht15CalibErr: 1;
   uint16_t I2cBusErr    : 1;
   uint16_t              : 0;
} ErrorFlags_t;

typedef union
{
   ErrorFlags_t ErrorsStruct;
   uint32_t ErrorsRawBits;
} GlobalErrorFlags_t;


//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//

/*!
 * \brief: Check if any error occured on i2c bus after sending cmd by esp
 */
void I2cBusErrCheck(esp_err_t Err);

/*!
 * \brief: Check presence of sensors error
 * \details: Write the errors from each sensor object struct to the global errors struct,
 *           then call GetErrId() and return that error ID
 */
ErrorId_t CheckErrors(void);


#endif