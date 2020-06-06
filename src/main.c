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
#include "event_loop.h"

#include "main.h"

// Enables certain modules for testing
//#define GPIO_T_TEST
//#define DAC_TEST // If defined, outputs 00 and 90 phase signals to DAC
//#define UART_TEST

// Globals
TIM_HandleTypeDef htim1 = {0}; // Handles PWM for h90_pwm00
TIM_HandleTypeDef htim9 = {0}; // Handles PWM for h00_pwm90

static uint32_t g_pwidth_temp = 0;
static uint32_t g_adcval_h90_pwm00 = PWM_PWIDTH_INIT;
static uint32_t g_adcval_h00_pwm90 = PWM_PWIDTH_INIT;

static uint32_t g_ADCbuf[ADC_BUFFER_SIZE];

static Hilbert_Buf g_hbuf = {0};

volatile uint32_t g_events = EVENTS_INIT;

//--------------------------------------

void event_set_pwm() {
    PWM_set_12bit(g_adcval_h90_pwm00, g_adcval_h00_pwm90);
}

void event_compute_hilbert() {
    // buffer value and calculate the hilbert outputs
    hilbert_record(&g_hbuf, g_pwidth_temp);
    g_adcval_h00_pwm90 = hilbert_phase_00_12bit(&g_hbuf);
    g_adcval_h90_pwm00 = hilbert_phase_90_12bit(&g_hbuf);
}

void event_adc_avg_half() {
    int i;

    // reset pwidth temp
    g_pwidth_temp = 0;

    // finds sum of first half of buffer (index 0 - 21)
    for (i = 0; i < ADC_BUFFER_HALF_SIZE; i++) {
        g_pwidth_temp += g_ADCbuf[i];
    }
}

void event_adc_avg_full() {
    int i;

    // adds sum of first to sum of second half of buffer (index 22 - 44)
    for (i = ADC_BUFFER_HALF_SIZE; i < ADC_BUFFER_SIZE; i++) {
        g_pwidth_temp += g_ADCbuf[i];
    }

    // get average value in 1/40kHz window
    g_pwidth_temp = g_pwidth_temp/ADC_BUFFER_SIZE;

#ifdef GPIO_T_TEST
    // GPIO toggle for testing
    HAL_GPIO_TogglePin(GPIO_TEST);
#endif
#ifdef DAC_TEST
    // Output to DAC for Testing
    DAC_set_channel_value(DAC_CHANNEL_00, g_pwidth_h00_pwm90);
    DAC_set_channel_value(DAC_CHANNEL_90, g_pwidth_h90_pwm00);
#endif
}

void process_events(uint32_t events) {
    if (!events)
        return;

    // Handle SET_PWM event
    if (EVENT_IS_SET(&g_events, EVENT_SET_PWM)) {
        event_set_pwm();

        RESET_EVENT(&g_events, EVENT_SET_PWM);
    }

    // Handle HILBERT_COMPUTE event
    else if (EVENT_IS_SET(&g_events, EVENT_HILBERT_COMPUTE)) {
        event_compute_hilbert();

        RESET_EVENT(&g_events, EVENT_HILBERT_COMPUTE);
    }

    // Handle ADC_AVG_FULL event for averaging second half of ADC buffer
    else if (EVENT_IS_SET(&g_events, EVENT_ADC_AVG_FULL)) {
        event_adc_avg_full();

        SET_EVENT(&g_events, EVENT_HILBERT_COMPUTE);
        RESET_EVENT(&g_events, EVENT_ADC_AVG_FULL);
    }

    // Handle ADC_AVG_HALF event for averaging first half of ADC buffer
    else if (EVENT_IS_SET(&g_events, EVENT_ADC_AVG_HALF)) {
        event_adc_avg_half();

        RESET_EVENT(&g_events, EVENT_ADC_AVG_HALF);
    }
}

int main(void) {
    __disable_irq();

    // System Initialization
    HAL_Init();
    SYSCLK_init();

    // Project Initialization
    GPIO_init();
    ADC1_init();

#ifdef UART_TEST
    UART_init();
#endif
#ifdef DAC_TEST
    DAC_init();
#endif

    // Timer Initialization
    MX_TIM3_init();
    MX_TIM1_init(&htim1);
    MX_TIM9_init(&htim9);
    TIM3_Interrupt_init();

    init_Hilbert_Buf(&g_hbuf);

#ifdef GPIO_T_TEST
    GPIO_init_pin(GPIO_TEST, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
#endif

    // Start
    TIM3_start_IT();
    PWM_start(&htim1, &htim9);
    ADC1_start_DMA(g_ADCbuf, ADC_BUFFER_SIZE);
    DAC_start();

    __enable_irq();

    while (1) {
        process_events(g_events);
    }
}
