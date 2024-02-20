/*
 * glog.h
 *
 *  Created on: Dec 15, 2021
 *      Author: seu
 */

#ifndef GLOG_H_
#define GLOG_H_

#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int log_level;
void gdebug(int lev, const char* fmt, ...);

int __io_putchar(int ch);
int __io_getchar(void);

int ggetc();
int gputc(int ch);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif /* GLOG_H_ */
