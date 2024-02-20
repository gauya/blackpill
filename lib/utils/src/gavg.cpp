
/*
 * gavg.cpp
 *
 *  Created on: 2021. 10. 14.
 *      Author: seu
 */

#include <gavg.h>

////////////////////////////////////////////////////////////////////////////

double gavg( gavg_t* k, double nv )
{
    k->v = ( (k->v*k->n) + nv ) / (k->n+1);
    if( k->n < k->cb ) k->n++;

    return k->v;
}

void set_gavg(gavg_t *a, uint8_t no) {
	a->n = 0;
	a->v = 0.;
	a->cb = no;
}

double last_gavg(gavg_t *a) {
	return a->v;
}
