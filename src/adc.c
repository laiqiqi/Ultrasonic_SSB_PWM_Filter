/*
 * adc.c
 *
 *  Created on: Feb 12, 2020
 *      Author: rstatz
 */

#include "adc.h"

#include "event_loop.h"

extern uint32_t g_events;

ADC_HandleTypeDef hadc1 = {0};

#define ADC1_SAMPLING_TIME ADC_SAMPLETIME_15CYCLES

// Called when ADC buffer is half full
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
    SET_EVENT(&g_events, EVENT_ADC_AVG_HALF);
}

// Called by HAL_ADC_IRQHandler when ADC buffer is full
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    SET_EVENT(&g_events, EVENT_ADC_AVG_FULL);
}

void ADC_IRQHandler() {
    HAL_ADC_IRQHandler(&hadc1);
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef* hadc) {
    Error_Handler();
}

void ADC1_interrupt_init() {
    HAL_NVIC_SetPriority(ADC_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
}

static void ADC_poll_test(ADC_HandleTypeDef* hadc) {
    uint32_t adcval;
    char strbuf[20] = {0};

    strcpy(strbuf, "adcval=");

    if (HAL_ADC_PollForConversion(hadc, 1000000) == HAL_OK) {
        adcval = HAL_ADC_GetValue(hadc);
        sprintf(strbuf + 7, "%ld\n", adcval);

        UART_Tx(strbuf);
    }
}

void ADC1_start_DMA(uint32_t* buf, uint32_t bufsize) {
    HAL_ADC_Start_DMA(&hadc1, buf, bufsize);
}

void ADC1_poll_test() {
    ADC_poll_test(&hadc1);
}

void ADC1_init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};

    // Start GPIO A CLK, may be redundancy
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_init_pin(GPIO_ADC_IN, GPIO_MODE_ANALOG, GPIO_NOPULL); // Initialize ADC Input Pin

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = DISABLE;

    // Start ADC CLK
    __HAL_RCC_ADC1_CLK_ENABLE();

    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC1_SAMPLING_TIME;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    // Initialize DMA
    DMA_ADC1_init(&hadc1);
}
