#include "pwm.h"

void PWM_config(TIM_HandleTypeDef* htim, uint32_t pulse, uint32_t channel) {
    TIM_OC_InitTypeDef sConfigOC = {0};

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.Pulse = pulse;

    HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, channel);
}

void PWM_asym_config(TIM_HandleTypeDef* htim, uint32_t pulse, uint32_t channel_pulse, uint32_t phase, uint32_t channel_phase) {
    TIM_OC_InitTypeDef sConfigOC = {0};

    sConfigOC.OCMode = TIM_OCMODE_ASSYMETRIC_PWM1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    sConfigOC.Pulse = pulse;
    HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, channel_pulse);

    sConfigOC.Pulse = phase;
    HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, channel_phase);
}
