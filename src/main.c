#include <stdio.h>
#include <string.h>

#include "dma.h"
#include "gpio.h"
#include "adc.h"
#include "dac.h"
#include "pwm.h"
#include "sysclk.h"
#include "uart.h"
#include "hilbert.h"
#include "error.h"

#include "main.h"

#define DAC_TEST // If defined, outputs 00 and 90 phase signals to DAC

#define TIM3_PERIOD_40KHZ 1350
#define TIM3_PERIOD TIM3_PERIOD_40KHZ

#define PWM_PRESCALE 0

#define PWM_PERIOD_40KHZ 5400
#define PWM_PERIOD PWM_PERIOD_40KHZ

#define PWM_PWIDTH_INIT PWM_PERIOD/2
#define PWM_PWIDTH_12BIT_RES_X1000 1318

#define PWM_PHASE_00 0
#define PWM_PHASE_90 PWM_PERIOD/4

// Globals
TIM_HandleTypeDef htim3 = {0}; // Handles 40KHz refresh

TIM_HandleTypeDef htim1 = {0}; // Handles PWM
TIM_HandleTypeDef htim4 = {0};

static uint32_t g_pwidth_temp = 0;
static uint32_t g_pwidth_00 = PWM_PWIDTH_INIT;
static uint32_t g_pwidth_90 = PWM_PWIDTH_INIT;

static uint32_t g_ADCbuf[ADC_BUFFER_SIZE];

static Hilbert_Buf g_hbuf = {0};

// Function Declarations
static void MX_TIM3_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);

//-------------------------------------
// Assumes pwidth is a 12 bit number (0 to 4095)
static void PWM_set_12bit(uint32_t pwidth1, uint32_t pwidth2) {
    uint32_t new_pwidth1, new_pwidth2; 

    // Convert 12bit number to PWM pulse width
    new_pwidth1 = (PWM_PWIDTH_12BIT_RES_X1000 * pwidth1)/1000;
    new_pwidth2 = (PWM_PWIDTH_12BIT_RES_X1000 * pwidth2)/1000;

    PWM_asym_config(&htim1, new_pwidth1, TIM_CHANNEL_1, PWM_PHASE_00, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

    PWM_asym_config(&htim1, new_pwidth2, TIM_CHANNEL_3, PWM_PHASE_90, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

void PWM_start() {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
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
//    __HAL_TIM_CLEAR_FLAG()
    __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE); // clear interrupt flag

    //PWM_set_12bit(g_pwidth_00, g_pwidth_90); TODO set back

    //	HAL_GPIO_TogglePin(GPIO_LED1);
//    HAL_GPIO_TogglePin(GPIO_TEST);
}

void PWM_test_osc() {
	static uint8_t dir = 1;

	g_pwidth_00 = dir ? g_pwidth_00 + 200 : g_pwidth_00 - 200;
    dir = ((g_pwidth_00 <= 200) || (g_pwidth_00 >= 4000)) ? (dir^1) : dir;

    delay(1000);
}

// Called when ADC buffer is half full
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
    int i;

    // reset pwidth temp
    g_pwidth_temp = 0;

    // finds sum of first half of buffer (index 0 - 21)
    for (i = 0; i < ADC_BUFFER_HALF_SIZE; i++) {
        g_pwidth_temp += g_ADCbuf[i];
    }
}

// Called when ADC buffer is full
// Called by HAL_ADC_IRQHandler
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    int i;

    // adds sum of first to sum of second half of buffer (index 22 - 44)
    for (i = ADC_BUFFER_HALF_SIZE; i < ADC_BUFFER_SIZE; i++) {
        g_pwidth_temp += g_ADCbuf[i];
    }

    // get average value in 1/40kHz window
    g_pwidth_temp = g_pwidth_temp/ADC_BUFFER_SIZE;

    // buffer value and calculate the hilbert outputs
    hilbert_record(&g_hbuf, g_pwidth_temp);
    g_pwidth_00 = hilbert_phase_0_12bit(&g_hbuf);
    g_pwidth_90 = hilbert_phase_90_12bit(&g_hbuf);

#ifdef DAC_TEST
    // Output to DAC for Testing
    DAC_set_channel_value(DAC_CHANNEL_00, g_pwidth_00);
    DAC_set_channel_value(DAC_CHANNEL_90, g_pwidth_90);
#endif

    // UART output for testing
//    char strbuf[1000];
//    sprintf(strbuf, "%ld,%ld\n", g_pwidth_00, g_pwidth_90);
//    UART_Tx(strbuf);

    // GPIO toggle for testing
    HAL_GPIO_TogglePin(GPIO_TEST);
}

int main(void) {
    __disable_irq();

    HAL_Init();
    SYSCLK_init();

    GPIO_init();
    UART_init();
    ADC1_init();

#ifdef DAC_TEST
    DAC_init();
#endif

    MX_TIM3_Init();
    TIM3_Interrupt_Init();
    MX_TIM1_Init();
    MX_TIM4_Init();

    init_Hilbert_Buf(&g_hbuf);

    GPIO_init_pin(GPIO_TEST, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);

    TIM3_start_IT();
    PWM_start();
    ADC1_start_DMA(g_ADCbuf, ADC_BUFFER_SIZE);
    DAC_start();

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
    __HAL_RCC_TIM3_CLK_ENABLE();

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



static void MX_TIM1_Init(void) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = PWM_PRESCALE;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = PWM_PERIOD;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    __HAL_RCC_TIM1_CLK_ENABLE();


    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
        Error_Handler();
    }

    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
    sSlaveConfig.InputTrigger = TIM_TS_ITR3;
    if (HAL_TIM_SlaveConfigSynchro(&htim1, &sSlaveConfig) != HAL_OK)
    {
     Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
      Error_Handler();
    }

    // Configure PWM Channels and set initial Pulse Width
    PWM_config(&htim1, PWM_PWIDTH_INIT, TIM_CHANNEL_1);

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
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK) {
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

static void MX_TIM4_Init(void) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = PWM_PRESCALE;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = PWM_PERIOD;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    __HAL_RCC_TIM4_CLK_ENABLE();

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim4) != HAL_OK) {
        Error_Handler();
    }

    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE;
    sSlaveConfig.InputTrigger = TIM_TS_ITR3;
    if (HAL_TIM_SlaveConfigSynchro(&htim4, &sSlaveConfig) != HAL_OK)
    {
      Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
    {
      Error_Handler();
    }

    PWM_config(&htim4, PWM_PWIDTH_INIT, TIM_CHANNEL_1);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = PWM_OUT_90_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(PWM_OUT_90_Port, &GPIO_InitStruct);
}
