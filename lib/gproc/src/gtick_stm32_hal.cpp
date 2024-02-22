/*
 * gtick_stm32_hal.c
 *
 *  Created on: Dec 15, 2021
 *      Author: seu
 */

//#include <main.h>
#include <gtick_stm32_hal.h>
#include <gproc.h>

#ifdef __cplusplus
extern "C" {
#endif

static eTick_vol __tick_vol = eTICK_VOL_100us;
static uint32_t __usec_per_tick;
static uint32_t __usec_max;
static double __ticks_per_usec;

static __IO uint32_t __STicks = 0;
static __IO uint32_t __mTicks = 0;
static __IO uint32_t __uTicks = 0;

inline uint32_t get_tick() {
	return (uint32_t)(SysTick->VAL & 0xffffff);
}

inline uint32_t get_utick() {
	return (uint32_t)(get_tick() / __ticks_per_usec);
}

uint32_t HAL_GetTick(void) {
	return __mTicks;
}

#if ( __RT_PROC__ != 0)
__attribute((__weak__)) void run_rtproc_all() {
	return;
}

__attribute((__weak__)) void run_rtproc(uint16_t i) {
	return;
}
#endif

void HAL_IncTick(void) {
	static uint16_t cnt=0, mcnt=0;
	__uTicks += __usec_per_tick;
	if(++cnt >= __tick_vol) {
		__mTicks++;
		cnt = 0;
		if(++mcnt > 1000) {
			__STicks++;
			mcnt = 0;
		}
	}

#if ( __RT_PROC__ != 0 )
	// check rtrun proc
	if( ! isinrtp() ) return;

// Adjusted to process once per msec regardless of clock
	__disable_irq();
	switch(__tick_vol) {
	case eTICK_VOL_1ms:
		run_rtproc_all();
		break;
	case eTICK_VOL_100us:
		run_rtproc(cnt);
		break;
	case eTICK_VOL_10us:
		if( (cnt%10) == 0 ) {
			run_rtproc(cnt /10);
		}
		break;
	case eTICK_VOL_1us:
		if( (cnt%100) == 0 ) {
			run_rtproc(cnt/100);
		}
		break;
	}
	__enable_irq();
#endif	
}

#ifdef __DEBUG__
#include "glog.h"
#endif

void init_ticks(eTick_vol vol) {
	__tick_vol = vol; // eTICK_VOL_10us; //eTICK_VOL_1us; // eTICK_VOL_1ms; // default eTICK_VOL_100us
	__usec_per_tick = 1000 / __tick_vol;
	__mTicks = 0;
	__uTicks = 0;

	HAL_SYSTICK_Config(SystemCoreClock / 1000U / __tick_vol);
	__usec_max = (uint32_t)(SysTick->LOAD & 0xffffff); // + 1;
	__ticks_per_usec = __usec_max / (double)(__usec_per_tick);

#ifdef __DEBUG__
	gdebug(100,"__tick_vol=%d, __usec_max=%lu, __usec_per_tick=%lu, __ticks_per_usec=%5.2f %lu/%lu\n"
			,__tick_vol, __usec_max, __usec_per_tick, __ticks_per_usec,__mTicks,__uTicks);
#endif
}

uint32_t get_stime() {
	return __STicks;
}

uint32_t get_mtime() {
	return __mTicks;
}

uint32_t get_utime() {
	return (__uTicks + (__usec_per_tick - get_utick()));
}

#ifdef __cplusplus
}
#endif
