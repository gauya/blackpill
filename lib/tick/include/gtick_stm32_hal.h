<<<<<<< HEAD
/*
 * gtick_stm32_hal.h
 *
 *  Created on: Dec 15, 2021
 *      Author: seu
 */

#ifndef __GTICK_STM32_HAL_H__
#define __GTICK_STM32_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INC_GTICK_STM32_HAL_H_
#define INC_GTICK_STM32_HAL_H_

#include <stdint.h>
#include <stm32f4xx_hal.h>

typedef enum  { // 1:1ms, 10:100us, 100:10us, 1000:1us
	eTICK_VOL_1ms = 1,
	eTICK_VOL_100us = 10,
	eTICK_VOL_10us = 100,
	eTICK_VOL_1us = 1000
} eTick_vol;

void init_ticks(eTick_vol);
uint32_t get_utick();

#endif /* INC_GTICK_STM32_HAL_H_ */

#ifdef __cplusplus
}
#endif

#endif //__GTICK_STM32_HAL_H__
=======
/*
 * gtick_stm32_hal.h
 *
 *  Created on: Dec 15, 2021
 *      Author: seu
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INC_GTICK_STM32_HAL_H_
#define INC_GTICK_STM32_HAL_H_

#include "stdint.h"

typedef enum  { // 1:1ms, 10:100us, 100:10us, 1000:1us
	eTICK_VOL_1ms = 1,
	eTICK_VOL_100us = 10,
	eTICK_VOL_10us = 100,
	eTICK_VOL_1us = 1000
} eTick_vol;

void init_ticks(eTick_vol);
uint32_t get_utick();

#endif /* INC_GTICK_STM32_HAL_H_ */

#ifdef __cplusplus
}
#endif
>>>>>>> 8cc7fef95d838e9e191f2e03980cbadb5dd857b4
