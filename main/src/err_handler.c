/*!
******************************************************************
* \file: err_handler.c
* \date: 09.10.23
* \author: Jacek Twardoch
* \brief:
******************************************************************
*/

//======================================[INCLUDES]======================================//
#include "err_handler.h"
#include "main.h"
//==================================[EXTERN VARIABLES]==================================//
//=============================[PRIVATE MACROS AND DEFINES]=============================//
#define MAX_ERR_NUM 5
//==================================[PRIVATE TYPEDEFS]==================================//
//==================================[STATIC VARIABLES]==================================//
//==================================[GLOBAL VARIABLES]==================================//
GlobalErrorFlags_t GlobalErrorsTable = { 0 };
//=============================[LOCAL FUNCTION PROTOTYPES]==============================//
//==================================[LOCAL FUNCTIONS]===================================//
//==================================[GLOBAL FUNCTIONS]==================================//


ErrorId_t CheckErrors(void)
{

   uint8_t err_counter = 0;
   ErrorId_t err_id = 0;
   const uint32_t errors_rawbits = GlobalErrorsTable.ErrorsRawBits;
   ErrorId_t errorID_position;

   for(errorID_position = ERROR_ID_POS_MIN; errorID_position < ERROR_ID_POS_MAX; errorID_position++)
   {
      if((errors_rawbits >> (errorID_position - 1)) & ERROR_PRESENT)
      {
         err_id = errorID_position;
         err_counter++;
      }
   }

   if(err_counter >= 2)
   {
      err_id = TooManyErrID;
   }

   /*Clear all errors*/
   GlobalErrorsTable.ErrorsRawBits = ERROR_NOT_PRESENT;

   return err_id;
}

void I2cBusErrCheck(esp_err_t Err)
{
   if((Err != ESP_OK))
   {
      GlobalErrorsTable.ErrorsStruct.I2cBusErr = ERROR_PRESENT;
   }
}
