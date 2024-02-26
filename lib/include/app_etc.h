
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
#include <gtick_stm32_hal.h>
#include <stm32yyxx_ll_iwdg.h>
#include "gtick.h"
#include "gutils.h"
#include "gprintf.h"

int add_proc(const char*pn, void (*f)(const char*), int timer=-1, int act=1);

#ifdef __cplusplus
extern "C" {
#endif

//void command_list(const char*str);
//void scadule_traffic(const char*);
void system_reset(const char *s);


#ifdef __cplusplus
}
#endif

#endif /* LIB_INC_APP_ETC_H_ */
