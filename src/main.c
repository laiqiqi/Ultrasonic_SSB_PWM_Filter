#include <stdio.h>
#include <string.h>

#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "error.h"
#include "gpio.h"
#include "hilbert.h"
#include "pwm.h"
#include "sysclk.h"
#include "tim.h"
#include "uart.h"

#include "main.h"

//#define DAC_TEST // If defined, outputs 00 and 90 phase signals to DAC

// Globals
TIM_HandleTypeDef htim3 = {0}; // Handles 40KHz refresh

TIM_HandleTypeDef htim1 = {0}; // Handles PWM
TIM_HandleTypeDef htim9 = {0};

static uint32_t g_pwidth_temp = 0;
static uint32_t g_pwidth_h90_pwm00 = PWM_PWIDTH_INIT;
static uint32_t g_pwidth_h00_pwm90 = PWM_PWIDTH_INIT;

static uint32_t g_ADCbuf[ADC_BUFFER_SIZE];

static Hilbert_Buf g_hbuf = {0};

//-------------------------------------
// Assumes pwidth is a 12 bit number (0 to 4095)
static void PWM_set_12bit(uint32_t pwidth1, uint32_t pwidth2) {
    uint32_t new_pwidth1, new_pwidth2; 

    // Convert 12bit number to PWM pulse width
    new_pwidth1 = (PWM_PWIDTH_12BIT_RES_X1000 * pwidth1)/1000;
    new_pwidth2 = (PWM_PWIDTH_12BIT_RES_X1000 * pwidth2)/1000;

    PWM_config(&htim1, new_pwidth1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

    PWM_config(&htim9, new_pwidth2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
}

// Runs @ 40KHz (theoretically)
void TIM3_IRQHandler(void) {
    __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE); // clear interrupt flag

    PWM_set_12bit(g_pwidth_h90_pwm00, g_pwidth_h00_pwm90);

    // HAL_GPIO_TogglePin(GPIO_LED1);
    // HAL_GPIO_TogglePin(GPIO_TEST);
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

// Called by HAL_ADC_IRQHandler when ADC buffer is full
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
    g_pwidth_h00_pwm90 = hilbert_phase_00_12bit(&g_hbuf);
    g_pwidth_h90_pwm00 = hilbert_phase_90_12bit(&g_hbuf);

#ifdef DAC_TEST
    // Output to DAC for Testing
    DAC_set_channel_value(DAC_CHANNEL_00, g_pwidth_h00_pwm90);
    DAC_set_channel_value(DAC_CHANNEL_90, g_pwidth_h90_pwm00);
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

    // System Initialization
    HAL_Init();
    SYSCLK_init();

    // Project Initialization
    GPIO_init();
    UART_init();
    ADC1_init();

#ifdef DAC_TEST
    DAC_init();
#endif

    // Timer Initialization
    MX_TIM3_init(&htim3);
    MX_TIM1_init(&htim1);
    MX_TIM9_init(&htim9);
    TIM3_Interrupt_init();

    init_Hilbert_Buf(&g_hbuf);

    GPIO_init_pin(GPIO_TEST, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);

    // Start
    TIM3_start_IT(&htim3);
    PWM_start(&htim1, &htim9);
    ADC1_start_DMA(g_ADCbuf, ADC_BUFFER_SIZE);
    DAC_start();

    __enable_irq();

    while (1) {
        //PWM_test_osc();
        //ADC1_poll_test();

        delay(5000000);
    }
}
