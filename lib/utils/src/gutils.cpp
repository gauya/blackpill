/*
 * gutils.cpp
 *
 *  Created on: 2021. 10. 15.
 *      Author: seu
 */

#include <gutils.h>

////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

double topg( pg_t *p, double v ) {
	//assert( p && (p->max > p->min) );
	if( v > p->max ) {
		v = p->max;
	} else
	if( v < p->min ) {
		v = p->min;
	}
	return (v / ( p->max - p->min )); // -1 ~ 1
}

uint32_t dif_u32(uint32_t s,uint32_t e) {
	return (uint32_t)(s > e)? (uint32_t)(0xffffffff - s + e) : (e - s);
}

uint32_t dif_u32_limit(uint32_t s,uint32_t e,uint32_t max) {
	return (uint32_t)(s > e)? ((max-s) + e) : (e-s);
}

#ifdef __cplusplus
}

#endif // __cplusplus

