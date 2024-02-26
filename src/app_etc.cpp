/*
 * app_etc.cpp
 *
 *  Created on: Mar 25, 2022
 *      Author: seu
 */
#include "app_etc.h"

int add_proc(const char*pn, void (*f)(const char*), int timer, int act) { // when act = 0 and timer >= 0, the processor starts in a stopped state.
	int no;
	if( timer != -1 ) {
		no = add_pfn( timer, (void(*)())f, pn);
		if(act == 0) 
			pfn_stop(no);
	}
	if( pn && strlen(pn)>0 )
		set_tty_func(pn,f);

	return no;
}

#ifdef __cplusplus
extern "C" {
#endif

void scadule_pre() {
	//
}

void scadule_post() {
	//
}

#if 0
void command_list(const char*str) {
	// ls --> void tty_inner_cmd_list()
	// ls -f view_proc()
	char bf[32];
	str = get_token(str, bf, 32);
	if(!bf[0]) {
		tty_inner_cmd_list();
		return;
	}
	const char *cmds[] = { "-p","-a","-l",0 };
	switch( instrs(bf,cmds)) {
	case 1: case 2: // -a
		tty_inner_cmd_list();
	case 0: // -p
		view_proc_all();
		break;
	default:
		gdebug(2,0,"ls [-p | -a]\n");
	}
}


uint32_t suspicious_looptime = 1000 * 10; // 10ms

void scadule_loop_check() {
	if(get_scadule_laptime() > suspicious_looptime) {
		// stop current proc
		gpfn_t *g = get_curpfn();
		if(g) {
			gdebug(0,0,"\n!!! (%d)[%s]\n",g->no,(g->pname)? g->pname : "");
		}
	}
}

void scadule_traffic(const char *str) {
	scadule_info_t *sit = get_scadule_inf();
	if( !sit ) return;

	int d,h,m,s;
	uint32_t u = get_stime();
	s = u % 60; u -= s;
	m = (u % 3600) / 60; u -= m*60;
	h = (u % (24 * 3600)) / 3600; u -= (h * 3600);
	d = u / (3600 * 24);

	gdebug(2,0,"load %.1lfus (%.1lf ~ %.1lf)us ps=%d elapsed=%dday %02d.%02d.%02d\n"
			, sit->avg_long.v
			, sit->avg_long.max_v,sit->avg_long.min_v
			,sit->_runs
			,d,h,m,s
			);
// clear data,get_scadule_laptime()
	sit->avg_long.max_v = DBL_MIN;
	sit->avg_long.min_v = DBL_MAX;
}
#endif

void system_reset(const char *s) {
	NVIC_SystemReset();
}

int tty_direct_func(int ch) {
	switch(ch) {
	case 'l':
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
		break;
	default:
		  break;
	}
	return ch;
}

int tty_prev_func(int c) {
//	tty_func_key(c);
	return 0;
}


#ifdef __cplusplus
}
#endif

