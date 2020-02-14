/*
 * gpio.c
 *
 *  Created on: Feb 13, 2020
 *      Author: rstatz
 */
#include "gpio.h"

void GPIO_init_pin(GPIO_TypeDef *bus, uint32_t pin, uint32_t mode, uint32_t pull) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pull = pull;
    HAL_GPIO_Init(bus, &GPIO_InitStruct);
}

void GPIO_init(void) {
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    //  __HAL_RCC_GPIOG_CLK_ENABLE();
    //  __HAL_RCC_GPIOH_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, LED1_Pin|LED3_Pin|LED2_Pin, GPIO_PIN_RESET);

    // Init button
    //  GPIO_init_pin(GPIO_BTN1, GPIO_MODE_IT_RISING, GPIO_NOPULL);

    // Init LEDs
    GPIO_init_pin(GPIOB, LED1_Pin|LED3_Pin|LED2_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
}
