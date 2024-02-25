
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

int toph( pg_t *p, double v ) {
	//assert( p && (p->max > p->min) );
	if( v > p->max ) {
		p->max = v;
		return 1;
	} else
	if( v < p->min ) {
		p->min = v;
		return -1;
	}
	return 0;
}

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

/// @brief 
/// @param s 
/// @param e 
/// @return 
uint32_t dif_u32(uint32_t s,uint32_t e) {
	return (uint32_t)(s > e)? (uint32_t)(0xffffffff - s + e) : (e - s);
}

uint32_t dif_u32_limit(uint32_t s,uint32_t e,uint32_t max) {
	return (uint32_t)(s > e)? ((max-s) + e) : (e-s);
}

int bit_count(uint32_t word) {
	int cnt = 0;
	int i=32;

	while( word && i-- > 0 ) { 
		if( word & 1 ) cnt++;
		word >>= 1;
	}

	return cnt;
}


/// @brief 
/// @param k 
/// @param nv 
/// @return 
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

#ifdef __cplusplus
}

double gavg::put( double val ) {
    v = ( (v * n) + val) / (n + 1);
    if( n < cb ) n++;

    return v;
}

void gavg::set(uint32_t b) {
	cb = b;
	if( cb < n ) n = cb;
}

/// @brief 
/// @tparam T 
/// @tparam N 
/// @param val 
/// @return 
template<typename T, typename N>
T gtg<T,N>::put( T val ) {
    v = ( (v * n) + val) / (n + 1);
    if( n < cb ) n++;
}

template<typename T,typename N>
void gtg<T,N>::set( N b ) {
	cb = b;
	if( cb < n ) n = cb;
}

/// @brief 
/// @param no 
/// @param cs 
gam::gam(int no=0, int *cs=0) {
	if( no <= 0 ) {
		return;
	}
	_len = no;
	_gs = new gavg_t[_len];

	for( int i; i < _len; i++ ) {
		if(cs) {
			_gs[i].cb = *cs++;
		} else {
			_gs[i].cb = 0;
		}
		_gs[i].n = 0;
		_gs[i].v = 0.;
	}
}

gam::~gam() {
	if( _gs ) delete[] _gs;
	_len = 0;
	_gs = 0;
}

#endif // __cplusplus
