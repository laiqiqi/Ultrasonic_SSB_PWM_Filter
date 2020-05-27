// Timer Configurations for SSB_PWM Project
#ifndef TIM_H_
#define TIM_H_

#include "main.h"

#define TIM3_PERIOD_40KHZ 1350
#define TIM3_PERIOD TIM3_PERIOD_40KHZ

#define PWM_PRESCALE 0

#define PWM_PERIOD_40KHZ 5400
#define PWM_PERIOD PWM_PERIOD_40KHZ

#define PWM_PWIDTH_INIT PWM_PERIOD/2
#define PWM_PWIDTH_12BIT_RES_X1000 1318

#define PWM_PHASE_00 0
#define PWM_PHASE_90 PWM_PERIOD/4

void MX_TIM3_init(TIM_HandleTypeDef*);
void MX_TIM1_init(TIM_HandleTypeDef*);
void MX_TIM9_init(TIM_HandleTypeDef*);

void TIM3_Interrupt_init();
void TIM3_start_IT();

#endif /* INC_TIM_H_ */
