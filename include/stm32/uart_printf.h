/*
 * uart_console.h
 *
 *  Created on: 2021. 10. 21.
 *      Author: seu
 */

#ifndef SRC_UART_CONSOLE_H_
#define SRC_UART_CONSOLE_H_

#include <stdio.h>
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

int __io_putchar(int ch);
int __io_getchar(void);
int _write(int32_t file, uint8_t *ptr, int32_t len);

HAL_StatusTypeDef start_uart_console(UART_HandleTypeDef *phuart, int intr, int echo);
int ugetc();
void uart_echo( int on );

#ifdef __cplusplus
}
#endif

#endif /* SRC_UART_CONSOLE_H_ */
