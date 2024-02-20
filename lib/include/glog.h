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

#ifdef __cplusplus
}

namespace std {

class glog final {
public:
	glog();
	virtual ~glog();
};

} /* namespace std */

#endif // __cplusplus
#endif /* GLOG_H_ */
