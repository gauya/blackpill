#ifndef __GPRINTF_H
#define __GPRINTF_H

#define _MAX_GPFMTBUF	80
#define _MAX_GPBUF_SZ 128

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define gputc(c) serial_write((c))
//extern int serial_write(int c);
int gputc(int c);
int gputs(const char*);
int gvprintf( const char* fmt, va_list ap);
int gsnprintf( char *buf, int sz, const char* fmt, ...);
int gprintf( const char* fmt, ...);
int gvbprintf( char *buf, int sz, const char* fmt, va_list ap);

#ifdef __cplusplus
}
#endif

#endif //__GPRINTF_H
