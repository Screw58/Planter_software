/*!
******************************************************************
* \file:
* \date:
* \author:
* \brief:
******************************************************************/

//======================================[INCLUDES]======================================//
#include "gpio_user.h"
//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//

//==================================[GLOBAL VARIABLES]==================================//

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//

//==================================[LOCAL FUNCTIONS]===================================//

//==================================[GLOBAL FUNCTIONS]==================================//

void GPIO_SetPinHigh(gpio_num_t gpio)
{
   gpio_set_level(gpio, GPIO_LEVEL_HIGH);
}

void GPIO_SetPinLow(gpio_num_t gpio)
{
   gpio_set_level(gpio, GPIO_LEVEL_LOW);
}

void GPIO_TogglePin(gpio_num_t gpio, uint8_t gpio_current_level)
{
   gpio_current_level ^= 1;
   gpio_current_level &= 1;
   gpio_set_level(gpio, gpio_current_level);
}
//   pin_level ^= 1;
//   gpio_set_level(LED_RGB_VCC_PIN, pin_level);