#include "tim.h"

#include "sysclk.h"
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

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(htim3, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
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
    htim1->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    __HAL_RCC_TIM1_CLK_ENABLE();


    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(htim1, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(htim1) != HAL_OK) {
        Error_Handler();
    }

    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
    sSlaveConfig.InputTrigger = TIM_TS_ITR3;
    if (HAL_TIM_SlaveConfigSynchro(htim1, &sSlaveConfig) != HAL_OK)
    {
     Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(htim1, &sMasterConfig) != HAL_OK)
    {
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

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitStruct.Pin = PWM_OUT_00_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(PWM_OUT_00_Port, &GPIO_InitStruct);
}

void MX_TIM9_init(TIM_HandleTypeDef* htim9) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    htim9->Instance = TIM9;
    htim9->Init.Prescaler = PWM_PRESCALE;
    htim9->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim9->Init.Period = PWM_PERIOD;
    htim9->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim9->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    __HAL_RCC_TIM4_CLK_ENABLE();

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(htim9, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(htim9) != HAL_OK) {
        Error_Handler();
    }

    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE;
    sSlaveConfig.InputTrigger = TIM_TS_ITR3;
    if (HAL_TIM_SlaveConfigSynchro(htim9, &sSlaveConfig) != HAL_OK)
    {
      Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(htim9, &sMasterConfig) != HAL_OK)
    {
      Error_Handler();
    }

    PWM_config(htim9, PWM_PWIDTH_INIT, TIM_CHANNEL_1);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = PWM_OUT_90_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(PWM_OUT_90_Port, &GPIO_InitStruct);
}
