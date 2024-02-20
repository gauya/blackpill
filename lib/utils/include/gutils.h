
/*
 * gutils.h
 *
 *  Created on: 2021. 10. 15.
 *      Author: seu
 */

#ifndef GUTILS_H_
#define GUTILS_H_

#ifndef ABS
#define ABS(x)        ( ((x)<0)?-(x):(x) )
#define MAX(x,y)      ( (x)>(y)?(x):(y) )
#define MIN(x,y)      ( (x)<(y)?(x):(y) )
#endif

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// max > min
typedef struct {
	double	max; //
	double	min; //
} pg_t;

double topg( pg_t *p, double v );
uint32_t dif_u32(uint32_t s,uint32_t e);
uint32_t dif_u32_limit(uint32_t s,uint32_t e,uint32_t max);

#ifdef __cplusplus
}

#else // c

#ifndef bool
typedef enum {
	false = 0,
	true = 1
} bool;
#endif

#endif // cplusplus


#endif /* GUTILS_H_ */
