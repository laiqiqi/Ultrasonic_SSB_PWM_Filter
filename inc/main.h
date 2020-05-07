#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#define STLK_TX_Pin GPIO_PIN_8
#define STLK_RX_Pin GPIO_PIN_9
#define USER_Btn_Pin GPIO_PIN_13
#define USER_Btn_GPIO_Port GPIOC
#define PWM_OUT_00_Pin GPIO_PIN_5
#define PWM_OUT_00_Port GPIOE
#define PWM_OUT_90_Pin GPIO_PIN_6
#define PWM_OUT_90_Port GPIOE
#define ADC_IN_Pin GPIO_PIN_0
#define ADC_IN_GPIO_Port GPIOA
#define DAC1_OUT_Pin GPIO_PIN_4
#define DAC1_OUT_Port GPIOA
#define DAC2_OUT_Pin GPIO_PIN_5
#define DAC2_OUT_Port GPIOA
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOB

#define GPIO_ADC_IN ADC_IN_GPIO_Port, ADC_IN_Pin
#define GPIO_DAC1_OUT DAC1_OUT_GPIO_Port, DAC1_OUT_Pin
#define GPIO_DAC2_OUT DAC2_OUT_GPIO_Port, DAC2_OUT_Pin
#define GPIO_LED1 LED1_GPIO_Port, LED1_Pin
#define GPIO_LED3 LED3_GPIO_Port, LED3_Pin
#define GPIO_BTN1 USER_Btn_GPIO_Port, USER_Btn_Pin
#define GPIO_TEST GPIOC, GPIO_PIN_8
#define GPIO_PWM_OUT_00 PWM_OUT_00_Port, PWM_OUT_00_Pin
#define GPIO_PWM_OUT_90 PWM_OUT_90_Port, PWM_OUT_90_Pin


#ifdef __cplusplus
}
#endif

#endif
