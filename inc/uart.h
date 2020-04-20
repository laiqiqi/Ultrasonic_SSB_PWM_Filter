/*
 * uart.h
 *
 *  Created on: Feb 12, 2020
 *      Author: rstatz
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include <string.h>
#include "stm32f7xx_hal.h"

#include "error.h"

void UART_init();

// accepts null terminated string
void UART_Tx(char*);

#endif /* INC_UART_H_ */
