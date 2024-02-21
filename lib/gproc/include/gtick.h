/*
 * gtick.h
 *
 *  Created on: 2021. 10. 14.
 *      Author: seu
 */

#ifndef GTICK_H_
#define GTICK_H_

#include <stdint.h>
#include "gutils.h"
#include "gtick_stm32_hal.h"

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

// must be recreate get_mtime() and get_utime()
// __weak attribute is applied
uint32_t get_stime(void);
uint32_t get_mtime(void);
uint32_t get_utime(void);
uint32_t get_utick(void);
uint32_t get_tick(void);

void delay_ms(uint32_t msec);
void delay_us(uint32_t msec);

uint32_t elapsed_us(uint32_t m );
uint32_t belapsed_us(uint32_t *m );
uint32_t elapsed_ms(uint32_t m );
uint32_t belapsed_ms(uint32_t *m );

typedef struct {
	uint32_t	check;
	uint32_t	m;
} timed_t;

int is_timed(timed_t *t);
int is_timed_acm(timed_t *t); // accumulated by the elapsed time
void set_timed(timed_t *t, uint32_t c);

#ifdef __cplusplus
 }

class gtick : protected timed_t {
public:
	gtick();
	gtick( uint32_t val );
	virtual ~gtick();

	int timed();
	void set( uint32_t val );
	uint32_t elapsed(); // milisecond

	uint32_t get_tick() const;
	int timed( timed_t *t );
	void set( timed_t *t, uint32_t val );

	uint32_t elapsed( uint32_t t ); // milisecond
};

#endif // __cplusplus

#endif /* GTICK_H_ */
