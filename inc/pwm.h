#ifndef PWM_H
#define PWM_H

#include "main.h"

void PWM_config(TIM_HandleTypeDef* htim, uint32_t pulse, uint32_t channel);
void PWM_asym_config(TIM_HandleTypeDef* htim, uint32_t pulse, uint32_t channel_pulse, uint32_t phase, uint32_t channel_phase);

void PWM_test_osc(uint32_t*);

//****************************
// Project Specific Functions
void PWM_start(TIM_HandleTypeDef* htim_00,TIM_HandleTypeDef* htim_90);

#endif
