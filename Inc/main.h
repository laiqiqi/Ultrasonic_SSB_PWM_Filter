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
#define GPIO_LED1 LED1_GPIO_Port, LED1_Pin
#define GPIO_BTN1 USER_Btn_GPIO_Port, USER_Btn_Pin
#define GPIO_TEST GPIOC, GPIO_PIN_8
#define GPIO_PWM GPIOE, GPIO_PIN_5

#define STLK_TX_Pin GPIO_PIN_8
#define STLK_RX_Pin GPIO_PIN_9
#define USER_Btn_Pin GPIO_PIN_13
#define USER_Btn_GPIO_Port GPIOC
#define PWM_OUT_Pin GPIO_PIN_5
#define PWM_OUT_GPIO_Port GPIOE
#define ADC_IN_Pin GPIO_PIN_0
#define ADC_IN_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif
