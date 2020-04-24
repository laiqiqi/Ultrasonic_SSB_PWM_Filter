#ifndef _DAC_H_
#define _DAC_H_

#include "stm32f7xx_hal.h"
#include "error.h"

#include "main.h"

#define DAC_CHANNEL_00 DAC_CHANNEL_1
#define DAC_CHANNEL_90 DAC_CHANNEL_2

void DAC_init();
void DAC_start();

void DAC_set_channel_value(uint32_t, uint32_t);

#endif
