/*
 * adc.c
 *
 *  Created on: Feb 12, 2020
 *      Author: rstatz
 */

#include "adc.h"

ADC_HandleTypeDef hadc1;

void ADC1_init(void) {
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK) {
    Error_Handler();
  }

  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
    Error_Handler();
  }
}

static void ADC_start(ADC_HandleTypeDef* hadc) {
	HAL_ADC_Start(hadc);
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

void ADC1_start() {
	ADC_start(&hadc1);
}

void ADC1_poll_test() {
	ADC_poll_test(&hadc1);
}
