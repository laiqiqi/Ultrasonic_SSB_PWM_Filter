/*
 * error.c
 *
 *  Created on: Feb 12, 2020
 *      Author: rstatz
 */

#include "error.h"
#include "main.h"

// Turns on LED1 if error occurs
void Error_Handler(void)
{
  HAL_GPIO_WritePin(GPIO_LED3, 1);
}

