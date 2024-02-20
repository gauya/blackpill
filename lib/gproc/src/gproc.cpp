<<<<<<< HEAD
/*
 * pfns.c
 *
 *  Created on: 2021. 12. 7.
 *      Author: seu
 */

#include <stdlib.h>
#include <gproc.h>
#include <glog.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	gpfn_t *list[MAX_PFNS+1];
//	uint32_t alive;
	int pfns;
} pfnlist_t;

#if ( __RT_PROC__ != 0)
typedef struct {
	grtpfn_t *list[MAX_RTPFNS+1];
	int rtpfns;
} rtpfnlist_t;
static rtpfnlist_t __rtp;
bool isinrtp() { return (__rtp.rtpfns > 0); }

#endif

static pfnlist_t __stp;

static uint16_t __restrict_time=0;
//static double __busyrate=0.0;

int init_pfn() {
	for(int i=0;i<MAX_PFNS; i++) {
		__stp.list[i] = 0;
	}
	__stp.pfns = 0;

#if ( __RT_PROC__ != 0)
	for(int i=0;i<MAX_RTPFNS; i++) {
		__rtp.list[i] = 0;
	}
	__rtp.rtpfns = 0;
#endif

	return 0;
}

int add_pfn(int prot, void (*pfn)(), const char*pname) {
	gpfn_t *g = (gpfn_t *)malloc(sizeof(gpfn_t));
	if( !g ) return -1;

	g->no = __stp.pfns;
	g->prot = prot;
	g->pfn = pfn;
	g->status = 0;
	if( prot > 0) {
		set_timed(&g->tmd,prot);
	}
	if(pname) { 
		strncpy(g->pname, pname, MAX_PFN_NAME_LEN);
		g->pname[MAX_PFN_NAME_LEN] = '\0';
	} else {
		g->pname[0] = 0;
	}
	__stp.list[__stp.pfns] = g;

	__stp.pfns++;

	return (__stp.pfns - 1);
}

#if ( __RT_PROC__ != 0)

int add_rtpfn(int t, void (*rtpfn)()) {
	if( __rtp.rtpfns >= MAX_RTPFNS )
		return -1;

	grtpfn_t *r = (grtpfn_t *)malloc(sizeof(grtpfn_t));
	if( !r ) return -1;

	set_timed(&(r->tmd), t);
	r->rtpfn = rtpfn;
	__rtp.list[__rtp.rtpfns] = r;
	__rtp.rtpfns++;

	return __rtp.rtpfns;
}
void run_rtproc(uint16_t i) {
	if( i >= MAX_RTPFNS ) return;

	grtpfn_t *r = __rtp.list[i];

	if( r && is_timed(&(r->tmd))) {
		r->rtpfn();
	}
}

void run_rtproc_all() {
	if( __rtp.rtpfns == 0 ) return;

	for( int i=0; i<MAX_RTPFNS; i++ ) {
		run_rtproc( i );
	}
}

#endif

int pfn_setstopflag(int no, int stop) { // stop==1 then set stop, or reset
	gpfn_t *g = get_pfn(no);
	if( !g ) return -1;

	if( stop ) {
		g->status |= 0x2;
	} else {
		g->status &= ~0x2;
	}

	return 0;
}

int pfn_settimer(int no, int ms) {
	gpfn_t *g = get_pfn(no);
	if( !g ) return -1;

	if( ms > 0 ) {
		set_timed(&(g->tmd), ms);
		g->prot = ms;
	} else {
		g->prot = ms;
		set_timed(&(g->tmd), 0);
	}

	return 0;
}

static bool is_pfn(int no) {
	if( no < 0 || no >= __stp.pfns )
		return false;

	if( !__stp.list[no] || __stp.list[no]->status == -1 || ! __stp.list[no]->pfn )
		return false;
	else
		return true;
}

void view_proc_all() {
	int i;
	gpfn_t *g;
	grtpfn_t *r;
	for(i=0; i<MAX_PFNS; i++) {
		g = __stp.list[i];
		if( !g ) continue;
		gdebug(2,"%2d %7d '%s' %d %lu %s\n",g->no, g->prot, g->pname, g->status, g->load
				, (g->status == 0)? "Standby"
						: (g->status == 1)? "Running"
								: (g->status == 2)?  "Stoped"
										: (g->status == -1)? "Deleted"
												: "Unknown");
	}
#if ( __RT_PROC__ != 0)
	for(i=0; i<MAX_RTPFNS; i++) {
		r = __rtp.list[i];
		if( !r ) continue;
		gdebug(0,"%2d * %7dms rt proc\n", i,r->tmd.check);
	}
#endif

}

gpfn_t *get_proc_inf(int no) {
	gpfn_t *g;
	for(int i=0; i<MAX_PFNS; i++) {
		g = __stp.list[i];
		if(  g->no == no ) return g;
	}
	return (gpfn_t*)0;
}

void view_proc(int no) {
	gpfn_t *g = get_proc_inf(no);
	if(g)
		gdebug(2,"%2d %7d '%s' %d %lu %s\n",g->no, g->prot, g->pname, g->status, g->load);
}

static void proc(gpfn_t *g) {
	if( g->status ) {
		return;
	}
	g->status |= 1;

	uint32_t pstime = get_utime();
	g->pfn();
	g->load = elapsed_us(pstime);

	g->status &= ~(1);
}

static int run() {
	gpfn_t *g;
	int runs=0;
	int restrict_flag=0;
	int i;

	// run_rtproc_all();
	
	uint32_t stime = get_utime();
	for(i=0; i<__stp.pfns; i++) {
		if( is_pfn(i) == false ) continue;
		g = __stp.list[i];

 		if( g->prot <= 0 || (g->status & 0x3) ) continue; 
 		if(  is_timed(&(g->tmd)) ) {
 			proc(g);

			if( restrict_flag == 0) {
				if( elapsed_us(stime) > __restrict_time ) {
					restrict_flag = 1;
				}
			}
	 		runs++;
		}
	}
	if( restrict_flag ) return runs;

	// time left and not impotant jobs
	for(i=0; i<__stp.pfns; i++) {
		if( elapsed_us(stime) > __restrict_time ) {
			return runs;
		}

		g = __stp.list[i];
		if( !g ) continue;

 		if( g->prot <= 0 && (g->status & 0x3)==0 ) {
 			proc(g);

 			runs++;
		}
	}

	return runs;
}

gpfn_t *get_pfn( int id ) {
	if( !is_pfn(id) ) return NULL;

	return __stp.list[id];
}


static int __runs = 0;
static uint32_t __srun, __erun;

__attribute__((weak)) void scadule_pre() {
	__srun = get_utime();
	return;
}

static void __scadule_post() {
	__erun = get_utime();

	uint32_t e = dif_u32(__srun,__erun);
	if( e > __restrict_time) {
		gdebug(5,"elapsed %lu - %lu/%lu\n", e, __srun,__erun);
	}
	if( __runs < __stp.pfns ) {
		// readjustment, large load pfn
	} else {
//		delay_us(1000 - (__erun-__srun));
//		delay_us(1);
	}

	return;
}

__attribute__((weak)) void scadule_post() {
}

void scadule() {
	__restrict_time = LOOP_RESTRICT_TIME;

	while(1) {
		scadule_pre();

		__runs = run();

		__scadule_post();
		scadule_post();
	}
}

#ifdef __cplusplus
}
#endif
=======
/*
 * pfns.c
 *
 *  Created on: 2021. 12. 7.
 *      Author: seu
 */

#include <stdlib.h>
#include <gproc.h>
#include <glog.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	gpfn_t *list[MAX_PFNS+1];
//	uint32_t alive;
	int pfns;
} pfnlist_t;

#if ( __RT_PROC__ != 0)
typedef struct {
	grtpfn_t *list[MAX_RTPFNS+1];
	int rtpfns;
} rtpfnlist_t;
static rtpfnlist_t __rtp;
bool isinrtp() { return (__rtp.rtpfns > 0); }

#endif

static pfnlist_t __stp;

static uint16_t __restrict_time=0;
//static double __busyrate=0.0;

int init_pfn() {
	for(int i=0;i<MAX_PFNS; i++) {
		__stp.list[i] = 0;
	}
	__stp.pfns = 0;

#if ( __RT_PROC__ != 0)
	for(int i=0;i<MAX_RTPFNS; i++) {
		__rtp.list[i] = 0;
	}
	__rtp.rtpfns = 0;
#endif

	return 0;
}

int add_pfn(int prot, void (*pfn)(), const char*pname) {
	gpfn_t *g = (gpfn_t *)malloc(sizeof(gpfn_t));
	if( !g ) return -1;

	g->no = __stp.pfns;
	g->prot = prot;
	g->pfn = pfn;
	g->status = 0;
	if( prot > 0) {
		set_timed(&g->tmd,prot);
	}
	if(pname) { 
		strncpy(g->pname, pname, MAX_PFN_NAME_LEN);
		g->pname[MAX_PFN_NAME_LEN] = '\0';
	} else {
		g->pname[0] = 0;
	}
	__stp.list[__stp.pfns] = g;

	__stp.pfns++;

	return (__stp.pfns - 1);
}

#if ( __RT_PROC__ != 0)

int add_rtpfn(int t, void (*rtpfn)()) {
	if( __rtp.rtpfns >= MAX_RTPFNS )
		return -1;

	grtpfn_t *r = (grtpfn_t *)malloc(sizeof(grtpfn_t));
	if( !r ) return -1;

	set_timed(&(r->tmd), t);
	r->rtpfn = rtpfn;
	__rtp.list[__rtp.rtpfns] = r;
	__rtp.rtpfns++;

	return __rtp.rtpfns;
}
void run_rtproc(uint16_t i) {
	if( i >= MAX_RTPFNS ) return;

	grtpfn_t *r = __rtp.list[i];

	if( r && is_timed(&(r->tmd))) {
		r->rtpfn();
	}
}

void run_rtproc_all() {
	if( __rtp.rtpfns == 0 ) return;

	for( int i=0; i<MAX_RTPFNS; i++ ) {
		run_rtproc( i );
	}
}

#endif

int pfn_setstopflag(int no, int stop) { // stop==1 then set stop, or reset
	gpfn_t *g = get_pfn(no);
	if( !g ) return -1;

	if( stop ) {
		g->status |= 0x2;
	} else {
		g->status &= ~0x2;
	}

	return 0;
}

int pfn_settimer(int no, int ms) {
	gpfn_t *g = get_pfn(no);
	if( !g ) return -1;

	if( ms > 0 ) {
		set_timed(&(g->tmd), ms);
		g->prot = ms;
	} else {
		g->prot = ms;
		set_timed(&(g->tmd), 0);
	}

	return 0;
}

static bool is_pfn(int no) {
	if( no < 0 || no >= __stp.pfns )
		return false;

	if( !__stp.list[no] || __stp.list[no]->status == -1 || ! __stp.list[no]->pfn )
		return false;
	else
		return true;
}

void view_proc_all() {
	int i;
	gpfn_t *g;
	grtpfn_t *r;
	for(i=0; i<MAX_PFNS; i++) {
		g = __stp.list[i];
		if( !g ) continue;
		gdebug(0,"%2d %7d '%s' %d %lu %s\n",g->no, g->prot, g->pname, g->status, g->load
				, (g->status == 0)? "Standby"
						: (g->status == 1)? "Running"
								: (g->status == 2)?  "Stoped"
										: (g->status == -1)? "Deleted"
												: "Unknown");
	}
#if ( __RT_PROC__ != 0)
	for(i=0; i<MAX_RTPFNS; i++) {
		r = __rtp.list[i];
		if( !r ) continue;
		gdebug(0,"%2d * %7dms rt proc\n", i,r->tmd.check);
	}
#endif

}

static void proc(gpfn_t *g) {
	if( g->status ) {
		return;
	}
	g->status |= 1;

	uint32_t pstime = get_utime();
	g->pfn();
	g->load = elapsed_us(pstime);

	g->status &= ~(1);
}

static int run() {
	gpfn_t *g;
	int runs=0;
	int restrict_flag=0;
	int i;

	// run_rtproc_all();
	
	uint32_t stime = get_utime();
	for(i=0; i<__stp.pfns; i++) {
		if( is_pfn(i) == false ) continue;
		g = __stp.list[i];

 		if( g->prot <= 0 || (g->status & 0x3) ) continue; 
 		if(  is_timed(&(g->tmd)) ) {
 			proc(g);

			if( restrict_flag == 0) {
				if( elapsed_us(stime) > __restrict_time ) {
					restrict_flag = 1;
				}
			}
	 		runs++;
		}
	}
	if( restrict_flag ) return runs;

	// time left and not impotant jobs
	for(i=0; i<__stp.pfns; i++) {
		if( elapsed_us(stime) > __restrict_time ) {
			return runs;
		}

		g = __stp.list[i];
		if( !g ) continue;

 		if( g->prot <= 0 && (g->status & 0x3)==0 ) {
 			proc(g);

 			runs++;
		}
	}

	return runs;
}

gpfn_t *get_pfn( int id ) {
	if( !is_pfn(id) ) return NULL;

	return __stp.list[id];
}


static int __runs = 0;
static uint32_t __srun, __erun;

__attribute__((weak)) void scadule_pre() {
	__srun = get_utime();
	return;
}

static void __scadule_post() {
	__erun = get_utime();

	uint32_t e = dif_u32(__srun,__erun);
	if( e > __restrict_time) {
		gdebug(5,"elapsed %lu - %lu/%lu\n", e, __srun,__erun);
	}
	if( __runs < __stp.pfns ) {
		// readjustment, large load pfn
	} else {
//		delay_us(1000 - (__erun-__srun));
//		delay_us(1);
	}

	return;
}

__attribute__((weak)) void scadule_post() {
}

void scadule() {
	__restrict_time = LOOP_RESTRICT_TIME;

	while(1) {
		scadule_pre();

		__runs = run();

		__scadule_post();
		scadule_post();
	}
}

#ifdef __cplusplus
}
#endif
>>>>>>> 8cc7fef95d838e9e191f2e03980cbadb5dd857b4
