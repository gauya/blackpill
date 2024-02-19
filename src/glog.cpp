/*
 * glog.cpp
 *
 *  Created on: Dec 15, 2021
 *      Author: seu
 */

#include <glog.h>

#include <stdarg.h>
#include <stdio.h>

int log_level = 2;

#ifdef ARDUINO
#include <Arduino.h>

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

#ifdef __cplusplus

namespace std {

glog::glog() {
	// TODO Auto-generated constructor stub

}

glog::~glog() {
	// TODO Auto-generated destructor stub
}

} /* namespace std */

#endif
