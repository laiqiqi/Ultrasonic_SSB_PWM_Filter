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

#include "dma.h"
#include "uart.h"
#include "gpio.h"
#include "error.h"

#include "main.h"

#define ADC_BUFFER_SIZE 45

void ADC1_init();
void ADC1_start_DMA(uint32_t*, uint32_t);

void ADC1_poll_test();

#endif /* INC_ADC_H_ */
