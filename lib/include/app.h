/*
 * user_main.h
 *
 *  Created on: 2021. 12. 8.
 *      Author: seu
 */

#ifndef INC_USER_MAIN_H_
#define INC_USER_MAIN_H_

#include "main.h"
#include "app_etc.h"

#include "stm32/guart_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void setup(UART_HandleTypeDef*);
void loop();

#ifdef __cplusplus
}
#endif

#endif /* INC_USER_MAIN_H_ */
