/*
 * glog.cpp
 *
 *  Created on: Dec 15, 2021
 *      Author: seu
 */

#include <glog.h>

#include <stdarg.h>
#include <stdio.h>

#ifdef ARDUINO
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

int log_level = 2;

void gdebug(int lev, const char *fmt, ...){ 
	if( lev > log_level ) return;

	va_list ap; 
	va_start(ap, fmt);
	int n = vsnprintf(NULL, 0, fmt, ap);
	char *str = new char[n+1];
	vsprintf(str, fmt, ap);
	va_end(ap);

	Serial.print(str);
	delete [] str;
}

int ggetc() {
	return Serial.read();
}

int gputc(int ch) {
	return Serial.write(ch);
}

#ifdef __cplusplus
}
#endif

#else
int gdebug(int lev, const char* fmt, ...) {
	if( lev > log_level ) return 1;

	char buf[128];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buf,127,fmt,ap);
	va_end(ap);

	printf(buf);
	return 0;
}

#endif
