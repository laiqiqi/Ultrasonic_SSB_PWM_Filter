#include "pwm.h"

#include "sysclk.h"

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

// Varies pulse width of PWM channel for testing purposes
void PWM_test_osc(uint32_t* g_pwidth_00) {
    static uint8_t dir = 1;

    *g_pwidth_00 = dir ? *g_pwidth_00 + 200 : *g_pwidth_00 - 200;
    dir = ((*g_pwidth_00 <= 200) || (*g_pwidth_00 >= 4000)) ? (dir^1) : dir;

    delay(1000);
}

//****************************
// Project Specific Functions

void PWM_start(TIM_HandleTypeDef* htim_00, TIM_HandleTypeDef* htim_90) {
    HAL_TIM_PWM_Start(htim_00, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(htim_90, TIM_CHANNEL_1);
}
