/*
 * pfns.h
 *
 *  Created on: 2021. 12. 8.
 *      Author: seu
 */

#include <stdlib.h>
#include <string.h>
#include <gtick.h>

#ifndef INC_PFNS_H_
#define INC_PFNS_H_

#define MAX_PFNS 32
#define MAX_PFN_NAME_LEN 15
#define MAX_RTPFNS 8

// usecond
#define LOOP_RESTRICT_TIME (20000 - 100)

#define __RT_PROC__ 0


//#include "stm32f1xx_hal.h"

typedef struct {
	int no;
	int prot; // 1~ : realtime
	int status;
	timed_t tmd;
	uint32_t load; // usec
	void (*pfn)();
	char pname[MAX_PFN_NAME_LEN+1];
} gpfn_t;

typedef struct {
	timed_t tmd;
	uint32_t load;
	void (*rtpfn)();
} grtpfn_t;

#ifdef __cplusplus
extern "C" {
#endif

int init_pfn();
int add_pfn(int prot, void (*pfn)(), const char*pname=0); // prot 0: any time any more

#if (__RT_PROC__ != 0 )
int add_rtpfn(int t, void (*rtpfn)());
bool isinrtp();
#endif

gpfn_t *get_pfn( int id );
int pfn_setstopflag(int no, int stop=1);
int pfn_settimer(int no, int ms);
void view_proc_all();

void scadule();

//void scadule_pre();
//void scadule_post();

#ifdef __cplusplus
}
#endif

#endif /* INC_PFNS_H_ */
