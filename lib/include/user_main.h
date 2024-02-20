
/*
 * user_main.h
 *
 *  Created on: 2021. 12. 8.
 *      Author: seu
 */

#ifndef INC_USER_MAIN_H_
#define INC_USER_MAIN_H_

#include "main.h"
#define UART_INTERRUPT
#include "stm32/uart_printf.h"

#ifdef __cplusplus
extern "C" {
#endif

void setup(UART_HandleTypeDef*);
void loop();

#ifdef __cplusplus
}
#endif

#endif /* INC_USER_MAIN_H_ */
