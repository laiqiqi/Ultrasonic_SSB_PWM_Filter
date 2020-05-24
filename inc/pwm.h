#ifndef PWM_H
#define PWM_H

#include "main.h"

void PWM_config(TIM_HandleTypeDef* htim, uint32_t pulse, uint32_t channel);
void PWM_asym_config(TIM_HandleTypeDef* htim, uint32_t pulse, uint32_t channel_pulse, uint32_t phase, uint32_t channel_phase);

#endif
