/*
 * gtick.cpp
 *
 *  Created on: 2021. 10. 14.
 *      Author: seu
 */

#include <gtick.h>
#include <giwdg.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute ((__weak__)) uint32_t get_stime() {
	return 0;
}

__attribute ((__weak__)) uint32_t get_mtime() {
//	 return HAL_GetTick();
	return 0;
}

__attribute ((__weak__)) uint32_t get_utime() {
	return 0;
}

__attribute ((__weak__)) uint32_t get_utick() {
	//	uint32_t ticks = SysTick->VAL;

	return 0;
}

__attribute ((__weak__)) uint32_t get_tick() {
	return 0;
}

int is_timed(timed_t *t) {
	if(t->check != 0 && elapsed_ms(t->m) >= t->check) {
		t->m = get_mtime(); 
		return 1;
	}
	return 0;
}

int is_timed_acm(timed_t *t) {
	if(t->check != 0 && elapsed_ms(t->m) >= t->check) {
		t->m += t->check;
		return 1;
	}
	return 0;
}

void set_timed(timed_t *t, uint32_t c) {
	t->check = c;
	t->m = get_mtime();
}

#define U32ALLSET (uint32_t)(-1)
//#define U32ALLSET (uint32_t)(0xffffffff)
void delay_ms( uint32_t ms ) {
#if 0
	ms += get_mtime();
	while(get_mtime() < ms);
#else
	uint32_t cm = get_mtime();
	if( (U32ALLSET - cm) < ms ) {
		while( get_mtime() >= cm );
		ms -= (U32ALLSET - cm);
	} else {
		ms += cm;
	}
	clear_iwdg();

	while(get_mtime() < ms) {
		clear_iwdg();
	}
#endif
}

void delay_us( uint32_t us ) {
#if 0
	us += get_utime();
	while(get_utime() < us);
#else
	uint32_t cu = get_utime();
	if( (U32ALLSET - cu) < us ) {
		while( get_utime() >= cu );
		us -= (U32ALLSET - cu);
	} else {
		us += cu;
	}
	while(get_utime() < us) clear_iwdg();
#endif
}
#undef U32ALLSET

uint32_t elapsed_us( uint32_t e ) {
	return dif_u32(e,get_utime());
}

uint32_t belapsed_us( uint32_t* e ) {
	uint32_t d = *e;
	*e = get_utime();
	return dif_u32(d,*e);
}

uint32_t elapsed_ms( uint32_t e ) {
	return dif_u32(e,get_mtime());
}

uint32_t belapsed_ms( uint32_t* e ) {
	uint32_t d = *e;
	*e = get_mtime();
	return dif_u32(d,*e);
}

#ifdef __cplusplus
}

gtick::gtick() {
	// TODO Auto-generated constructor stub
	check = 0;
	m = get_tick();
}

gtick::gtick(uint32_t val) {
	check = val;
	m = get_tick();
}

gtick::~gtick() {
	// TODO Auto-generated destructor stub
}

uint32_t gtick::get_tick() const {
	return get_mtime();
}

int gtick::timed() {
	if(check > 0 && elapsed() >= check) {
		m = get_mtime();
		return 1;
	}
	return 0;
}
void gtick::set( uint32_t val ) {
	check = val;
	m = get_mtime();
}
uint32_t gtick::elapsed() { // milisecond
	return dif_u32(m,get_mtime());
}

int gtick::timed( timed_t *t ) {
	return ::is_timed(t);
}

void gtick::set( timed_t *t, uint32_t val ) {
	::set_timed(t, val);
}

uint32_t gtick::elapsed( uint32_t t ) { // milisecond
	return ::elapsed_ms( t );
}

#endif // __cplusplus

