/*
 * app_etc.h
 *
 *  Created on: Mar 25, 2022
 *      Author: seu
 */

#ifndef LIB_INC_APP_ETC_H_
#define LIB_INC_APP_ETC_H_

#include "app.h"
#include "gstr.h"
#include "glog.h"
#include "gtty.h"
#include "gproc.h"
#include "stm32/gpwr_hal.h"
#include <stm32/gtick_hal.h>
#include "gtick.h"
#include "gutil.h"
#include "gprintf.h"

#ifdef __cplusplus
int add_proc(const char*pn, void (*f)(const char*), int timer=-1, int act=1);

extern "C" {
#endif

void command_list(const char*str);
void system_reset(const char *s);
void scadule_traffic(const char*);
void clear_iwdg();

extern IWDG_HandleTypeDef hiwdg; // hiwdg1

#ifdef __cplusplus
}
#endif

#endif /* LIB_INC_APP_ETC_H_ */
