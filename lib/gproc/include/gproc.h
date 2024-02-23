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

#define __RT_PROC__ 1

#ifdef __cplusplus
extern "C" {
#endif


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

int init_pfn();
int add_pfn(int prot, void (*pfn)(), const char*pname=0,int act=1); // prot 0: any time any more

#if (__RT_PROC__ != 0 )

typedef struct {
	timed_t tmd;
	uint32_t load;
	void (*rtpfn)();
} grtpfn_t;

int add_rtpfn(int t, void (*rtpfn)());
bool isinrtp();

#endif // __RT_PROC__

struct scadule_trfinf {
	uint32_t _startms;
	uint64_t _looped;
	uint32_t _avg1us,_avg2us;
	uint32_t _maxus, _minus;
};

gpfn_t *get_pfn( int id );
int pfn_stop(int no, int stop=1);
int pfn_start(int no); 
int pfn_frq(int no, int ms);
gpfn_t *get_proc_inf(int no);
void view_proc(int no);
void view_proc_all();

void scadule();

//void scadule_pre();
//void scadule_post();

#ifdef __cplusplus
}
#endif

#endif /* INC_PFNS_H_ */
