/*
 * gpio.h
 *
 *  Created on: Feb 13, 2020
 *      Author: rstatz
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include "main.h"

void GPIO_init_pin(GPIO_TypeDef *bus, uint32_t pin, uint32_t mode, uint32_t pull);
void GPIO_init();


#endif /* INC_GPIO_H_ */
