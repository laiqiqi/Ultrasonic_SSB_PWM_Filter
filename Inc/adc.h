/*
 * adc.h
 *
 *  Created on: Feb 12, 2020
 *      Author: rstatz
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include <stdio.h>
#include <string.h>
#include "stm32f7xx_hal.h"

#include "uart.h"
#include "error.h"

void ADC1_init();
void ADC1_start();

void ADC1_poll_test();

#endif /* INC_ADC_H_ */
