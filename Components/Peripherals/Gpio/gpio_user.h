/*!
*******************************************************
* \file: gpio_user.h
* \date:
* \author:
* \brief:
********************************************************/
#ifndef GPIO_H_
#define GPIO_H_
//======================================[INCLUDES]======================================//
#include "driver/gpio.h"

//=================================[MACROS AND DEFINES]=================================//
#define GPIO_LEVEL_HIGH 1
#define GPIO_LEVEL_LOW  0
//======================================[TYPEDEFS]======================================//

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//

void GPIO_SetPinHigh(gpio_num_t gpio);

void GPIO_SetPinLow(gpio_num_t gpio);

void GPIO_TogglePin(gpio_num_t gpio, uint8_t *gpio_current_level);

void GPIO_VccLedPinInit(uint16_t pin_num);
#endif