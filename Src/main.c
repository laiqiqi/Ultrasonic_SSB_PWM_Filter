#include <stdio.h>
#include <string.h>

#include "dma.h"
#include "gpio.h"
#include "adc.h"
#include "sysclk.h"
#include "uart.h"
#include "error.h"

#include "main.h"

#define TIM3_PERIOD_40KHZ 1350
#define TIM3_PERIOD TIM3_PERIOD_40KHZ

#define PWM_PRESCALE 0
#define PWM_PWIDTH_12BIT_RES_X1000 1318
#define PWM_PWIDTH_INIT 3
#define PWM_PERIOD_40KHZ 5400
#define PWM_PERIOD PWM_PERIOD_40KHZ

TIM_HandleTypeDef htim3; // Handles 40KHz refresh
TIM_HandleTypeDef htim9; // Handles PWM

static uint32_t g_pwidth = PWM_PWIDTH_INIT;

static uint32_t g_ADCbuf[ADC_BUFFER_SIZE];

static void MX_TIM3_Init(void);
static void MX_TIM9_Init(void);

//-------------------------------------
// Assumes pwidth is a 12 bit number (0 to 4095)
static void PWM_set_12bit(uint32_t pwidth) {
    TIM_OC_InitTypeDef sConfigOC;
    uint32_t new_pwidth = (PWM_PWIDTH_12BIT_RES_X1000 * pwidth)/1000;

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = new_pwidth;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;

    HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
}

void PWM_start() {
//	 __HAL_RCC_TIM9_CLK_ENABLE();
    //HAL_TIM_Base_Start(&htim9);
    if (HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
}

void TIM3_start_IT() {
    HAL_TIM_Base_Start_IT(&htim3);
}

// Interrupt setup
void TIM3_Interrupt_Init(void) {
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

// Runs @ 40KHz (theoretically)
void TIM3_IRQHandler(void) {
    __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE); // clear interrupt flag

    PWM_set_12bit(g_pwidth);

    //	HAL_GPIO_TogglePin(GPIO_LED1);
}

void PWM_test_osc() {
	static uint8_t dir = 1;

	g_pwidth = dir ? g_pwidth + 200 : g_pwidth - 200;
    dir = ((g_pwidth <= 200) || (g_pwidth >= 4000)) ? (dir^1) : dir;

    delay(1000);
}

// Called by HAL_ADC_IRQHandler
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    int i;
    uint32_t pwidth_temp = 0;

    for (i = 0; i < ADC_BUFFER_SIZE; i++) {
        pwidth_temp += g_ADCbuf[i];
    }

    g_pwidth = pwidth_temp/ADC_BUFFER_SIZE;
//
//    sprintf(strbuf, "%ld\n", g_pwidth);
//
//    UART_Tx(strbuf);
}

int main(void) {
    __disable_irq();

    HAL_Init();
    SYSCLK_init();

    GPIO_init();
    UART_init();
    ADC1_init();

    MX_TIM3_Init();
    TIM3_Interrupt_Init();
    MX_TIM9_Init();

    GPIO_init_pin(GPIO_TEST, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);

    TIM3_start_IT();
    PWM_start();
    ADC1_start_DMA(g_ADCbuf, ADC_BUFFER_SIZE);

    //g_pwidth = 200;

    __enable_irq();

    while (1) {
        //PWM_test_osc();
        //ADC1_poll_test();

        delay(5000000);
    }
}

static void MX_TIM3_Init(void) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 0;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = TIM3_PERIOD;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_TIM9_Init(void) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim9.Instance = TIM9;
    htim9.Init.Prescaler = PWM_PRESCALE;
    htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim9.Init.Period = PWM_PERIOD;
    htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim9) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim9) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = PWM_PWIDTH_INIT;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }

    HAL_TIM_MspPostInit(&htim9);
}
