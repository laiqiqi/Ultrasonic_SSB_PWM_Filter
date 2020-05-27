#include "tim.h"

#include "error.h"
#include "pwm.h"

// Sets up TIM3 at 40kHz
void MX_TIM3_init(TIM_HandleTypeDef* htim3) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim3->Instance = TIM3;
    htim3->Init.Prescaler = 0;
    htim3->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3->Init.Period = TIM3_PERIOD;
    htim3->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    __HAL_RCC_TIM3_CLK_ENABLE();

    // Set timer clock source
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(htim3, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }

    // Triggers outputs that enable other timers
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(htim3, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
}

// Interrupt setup
void TIM3_Interrupt_init(void) {
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM3_start_IT(TIM_HandleTypeDef* htim3) {
    HAL_TIM_Base_Start_IT(htim3);
}

// Sets up TIM1 to start When TIM3 does
// Outputs PWM
void MX_TIM1_init(TIM_HandleTypeDef* htim1) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    htim1->Instance = TIM1;
    htim1->Init.Prescaler = PWM_PRESCALE;
    htim1->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1->Init.Period = PWM_PERIOD;
    htim1->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; // If enabled, Does not change register values until start of new cycle
    __HAL_RCC_TIM1_CLK_ENABLE();

    // Set timer clock source
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(htim1, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(htim1) != HAL_OK) {
        Error_Handler();
    }

    // Master and Slave configuration sets timer to start when TIM3 does via ITR2
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
    sSlaveConfig.InputTrigger = TIM_TS_ITR2;
    if (HAL_TIM_SlaveConfigSynchro(htim1, &sSlaveConfig) != HAL_OK) {
        Error_Handler();
    }

    // Configure PWM Channels and set initial Pulse Width
    PWM_config(htim1, PWM_PWIDTH_INIT, TIM_CHANNEL_1);

    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(htim1, &sBreakDeadTimeConfig) != HAL_OK) {
      Error_Handler();
    }

    // Initialize PWM output GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitStruct.Pin = PWM_OUT_00_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(PWM_OUT_00_Port, &GPIO_InitStruct);

    // Initialize counter value to create phase shift
    htim1->Instance->CNT = PWM_PHASE_00;
}

void MX_TIM9_init(TIM_HandleTypeDef* htim9) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    htim9->Instance = TIM9;
    htim9->Init.Prescaler = PWM_PRESCALE;
    htim9->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim9->Init.Period = PWM_PERIOD;
    htim9->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim9->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; // If enabled, Does not change register values until start of new cycle
    __HAL_RCC_TIM9_CLK_ENABLE();

    // Set timer clock source
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(htim9, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(htim9) != HAL_OK) {
        Error_Handler();
    }

    // Slave configuration sets timer to start when TIM3 does via ITR1
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
    sSlaveConfig.InputTrigger = TIM_TS_ITR1;
    if (HAL_TIM_SlaveConfigSynchro(htim9, &sSlaveConfig) != HAL_OK)
    {
      Error_Handler();
    }

    // Configure PWM Channels and set initial Pulse Width
    PWM_config(htim9, PWM_PWIDTH_INIT, TIM_CHANNEL_1);

    // Initialize PWM output GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Pin = PWM_OUT_90_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
    HAL_GPIO_Init(PWM_OUT_90_Port, &GPIO_InitStruct);

    // Initialize counter value to create phase shift
    htim9->Instance->CNT = PWM_PHASE_90;
}
