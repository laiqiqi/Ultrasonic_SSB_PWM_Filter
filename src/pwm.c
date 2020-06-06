#include "pwm.h"

#include "sysclk.h"
#include "tim.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim9;

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

// Assumes pwidth is a 12 bit number (0 to 4095)
void PWM_set_12bit(uint32_t hilbert_90, uint32_t hilbert_00) {
    uint32_t pwidth1, pwidth2;

    // Convert 12bit number to PWM pulse width
    pwidth1 = (PWM_PWIDTH_12BIT_RES_X1000 * hilbert_90)/1000;
    pwidth2 = (PWM_PWIDTH_12BIT_RES_X1000 * hilbert_00)/1000;

    PWM_config(&htim1, pwidth1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

    PWM_config(&htim9, pwidth2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
}

void PWM_start(TIM_HandleTypeDef* htim_00, TIM_HandleTypeDef* htim_90) {
    HAL_TIM_PWM_Start(htim_00, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(htim_90, TIM_CHANNEL_1);
}
