#include <gstr.h>
#include <glog.h>
#include <gtty.h>

typedef enum { 
	CMD_MODE_ECHO, 
	CMD_MODE_CMD, 
	CMD_MODE_ONLY_OUTPUT,
	CMD_MODE_MSG,
	CMD_MODE_DIRECT  
} teUCTYPE ;

typedef struct {
	const char *cmd;
	int no;
} inner_cmd_list;

typedef enum {
	eHELP,
	eLOGLEVEL,
//	eCMDS,
	eDIRECT
} tty_inner_enum;

static inner_cmd_list __icl[] = {
	{ "help", eHELP },
	{ "log",eLOGLEVEL},
//	{ "cmd",eCMDS},
	{ "direct", eDIRECT },
	{ NULL,}
};

static struct s_command {
	const char *cmd;
	void (*f)(const char*);
} __cmds[MAX_TTY_CMD_FUNC] = { {(const char*)0,0},};

int direct_escape_key = 0x3; // CTRL+C

static uint8_t next_cmdfunc_no = 0;

__attribute__((weak)) void help_tty_msg(void) {
	printf("ls -l\n... there is no help doc yet\n");
}

void tty_inner_cmd_list() {
	inner_cmd_list *i = __icl;
	while(i->cmd) {
		printf(TTY_PROMPT0" %s\n",i->cmd);
		i++;
	}
	struct s_command *s = __cmds;
	while(s->cmd) {
		printf(TTY_PROMPT" %s\n",s->cmd);
		s++;
	}
}

__attribute__((weak)) void command_list(const char*arg) {
	tty_inner_cmd_list();
}

static int default_inner_func(int c,const char *s) {
	int lev;
	switch(c) {
		case eDIRECT:
			return -100;
		case eHELP:
			help_tty_msg();
			break;
		case eLOGLEVEL:
			s = whitespace_skip(s);
			if( !s )
				lev = log_level;
			else
				lev = log_level = stoi(s);
			gdebug(-10,"log level is [%d]\n",lev);
			break;
		default:;
	}
	return 0;
}

__attribute__((weak)) int tty_prev_func(int c) {
	return 0;
}

__attribute__((weak)) int tty_direct_func(int c) {
	return 0;
}

/*
arrow key , also keypad, numeric keypad
up     : 1b5b41
down   : 1b5b42
right  : 1b5b43
left   : 1b5b44
home   : 1b5b317e
insert : 1b5b327e
delete : 1b5b337e
end    : 1b5b347e
pgup   : 1b5b357e
pgdn   : 1b5b367e
f1     : 1b5b5b41
f2     : 1b5b5b42
f3     : 1b5b5b43
f4     : 1b5b5b44
f5     : 1b5b5b45
f6     : 1b5b31377e
f7     : 1b5b31387e
f8     : 1b5b31397e
f9     : 1b5b32307e
f10    : 1b5b32317e
f11    : 1b5b32337e
f12    : 1b5b32347e
alt +  : 1b +   //ex, alt+f12 : 1b1b5b32347e
*/

int tty_func_key(int c) {
	static int mode=0,alt=0,skey=0;
//	gdebug(2,0,"[%04x]",c);

	if(mode == 0) {
		if(c == 0x1b) {
			mode = 1;
			return 1;
		}
		if(c == 0x04) { // ctrl+d
			return -100;
		}
		return 0;
	}
	if(mode == 1) {
		if(c == 0x5b) {
			mode = 2;
			return 1;
		} else
		if(c == 0x1b) {
			alt = 1;
			return 1;
		} else {
			mode = alt = 0;
			return 0;
		}
	}
	if(mode == 2) {
		if(c == 0x5b) {
			mode = 3;
			return 1;
		} else {
			mode = alt = 0;
			return 0;
		}
	}
	if(mode == 3) { // up,down,right,left
		switch(c) {
		case 0x5b:  // f1 ~ f5
			mode = 4;
			return 0;
		case 0x41:
			break;
		case 0x42:
			break;
		case 0x43:
			break;
		case 0x44:
			break;
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
			skey = c;
			break;
		default:;
		}
	}
	if(mode == 4) { // f1 ~ f5, home,insert,delete,end,pgup,pgdn
		switch(c) {
		case 0x41: // f1
			break;
		case 0x42:
			break;
		case 0x43:
			break;
		case 0x44:
			break;
		case 0x45:
			break;
		default:
			;
		}

		mode = skey = alt = 0;
		return 0;
	}
	if(mode == 5) { // f6 ~ f12
		if( c != 0x7e ) {
			// error, unknown
		}
	}

	mode = skey = alt = 0;
	return 0;
}


///////////////////////////////////////////////////////////////////
static int (*__default_tty_func)(const char*,int) = 0;

int set_tty_func(const char*cmdstr,void (*func)(const char *)) {
	if(cmdstr == NULL) {
		__default_tty_func = (int(*)(const char*,int))func;
	} else
	if( next_cmdfunc_no < (MAX_TTY_CMD_FUNC) )
	{
		__cmds[next_cmdfunc_no].cmd = cmdstr;
		__cmds[next_cmdfunc_no].f = func;
		next_cmdfunc_no++;
		return 1;
	}
	return 0;	
}

static int cmd_direct(int c) {
	if( c != direct_escape_key ) {
		return tty_direct_func(c);
	}
	return 0; // escape direct mode
}

static int iscmd(const char* c, const char* d) {
	while(*d && *c && *d == *c) {
		d++;c++;
	}
	return (*c)? 0 : 1; 
}

static int tty_cmdp(const char* s) {
	inner_cmd_list *il = __icl;
	
	s = whitespace_skip(s);
	if( !s || !*s ) return 0;
	
	while(il->cmd) {
		if( iscmd(il->cmd,s) ) {
			return default_inner_func(il->no,s+strlen(il->cmd));
		}
		il++;
	}
	struct s_command* cm;

	for(cm=__cmds; cm->cmd; cm++) {
		if( iscmd(cm->cmd,s) ) {
			if(cm->f) {
				cm->f( s+strlen(cm->cmd) );
			}
			return 0;
		}
	}
	return -2;  
}

static int tty_mode = CMD_MODE_ECHO;
static uint8_t __inb[TTY_LINE_BUF_SIZE+1];
static uint8_t __cbp = 0;

void tty() {
	int c = ggetc();
	if( c <= 0 ) {
		return;
	}
	if( c=='\r') {
		c = '\n';
	}
	gputc(c);

	int ret = 0;

	if( (ret = tty_prev_func(c)) != 0 ) {
		if( ret == -100 ) {
			__cbp = 0;
			__inb[__cbp] = '\0';
			tty_mode = CMD_MODE_DIRECT;
			gdebug(2,0,"set to direct mode(stop Ctrl+C)\r\n");
		}
		return;
	}

	switch ( tty_mode ) {
		case CMD_MODE_ONLY_OUTPUT:
			break;
		case CMD_MODE_MSG:
		case CMD_MODE_ECHO:
			if( c == '\n' ) {
				__inb[__cbp] = '\0';
				ret = tty_cmdp((const char*)__inb);
				if( ret == -2 ) {
					if(__default_tty_func) {
						__default_tty_func((const char*)__inb,__cbp);
					} else {
						gdebug(2,0,"[%s] Unknown command\r\n",__inb);
					}
				}
				else
				if( ret == -100 ) {
					tty_mode = CMD_MODE_DIRECT;
					gdebug(2,0,"set to direct mode(stop Ctrl+C)\n");
					// echo off
				}
				
				__cbp = 0;
				__inb[0] = '\0';
#ifdef USE_TTY_PROMPT
				gputc(TTY_PROMPT[0]);
#endif //USE_TTY_PROMPT				
			} else
			if( c < ' ' || c >= 127 ) { // \b, 0x08 backspace
				if(__cbp > 0) __cbp--;
			} else
			if(__cbp < (TTY_LINE_BUF_SIZE)) {
				__inb[__cbp++] = c;
			} else {
				// ignore
			}
				
			break;
		case CMD_MODE_DIRECT:
			if(cmd_direct(c) == 0) {
				// echo on
				gdebug(2,0,"\nend direct mode\n]");
				tty_mode = CMD_MODE_ECHO;
			}
//			__cbp = 0;
			break;
		default:
		;
	}
}
