<<<<<<< HEAD
/*
 * gavg.h
 *
 *  Created on: 2021. 10. 14.
 *      Author: seu
 */

#ifndef GAVG_H_
#define GAVG_H_

#define DEF_GAVG_CALIBRATION 10

#include <stdint.h>

typedef struct {
	uint8_t cb,n;
	double  v;
} gavg_t;

#ifdef __cplusplus
extern "C" {
#endif

double gavg(gavg_t *k, double nv );
void set_gavg(gavg_t *k, uint8_t no);
double last_gavg(gavg_t *a);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class gavg : public gavg_t {
public:
	gavg(uint8_t calibrate = DEF_GAVG_CALIBRATION) {
		v = 0.;
		n = 0;
		cb = calibrate;
	}
	virtual ~gavg();

	double &val() { return v; }
	double inv( double nv ) {
	    v = ( (v*n) + nv ) / (n+1);
	    if( n < cb ) n++;
	    return v;
	}
};

#endif // __cplusplus

#endif /* GAVG_H_ */
=======
/*
 * gavg.h
 *
 *  Created on: 2021. 10. 14.
 *      Author: seu
 */

#ifndef GAVG_H_
#define GAVG_H_

#define DEF_GAVG_CALIBRATION 10

#include <stdint.h>

typedef struct {
	uint8_t cb,n;
	double  v;
} gavg_t;

#ifdef __cplusplus
extern "C" {
#endif

double gavg(gavg_t *k, double nv );
void set_gavg(gavg_t *k, uint8_t no);
double last_gavg(gavg_t *a);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class gavg : public gavg_t {
public:
	gavg(uint8_t calibrate = DEF_GAVG_CALIBRATION) {
		v = 0.;
		n = 0;
		cb = calibrate;
	}
	virtual ~gavg();

	double &val() { return v; }
	double inv( double nv ) {
	    v = ( (v*n) + nv ) / (n+1);
	    if( n < cb ) n++;
	    return v;
	}
};

#endif // __cplusplus

#endif /* GAVG_H_ */
>>>>>>> 8cc7fef95d838e9e191f2e03980cbadb5dd857b4
