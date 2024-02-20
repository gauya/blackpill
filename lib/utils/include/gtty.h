#ifndef __GAU_TTY_H
#define __GAU_TTY_H

typedef enum  {
	esckey_up = 201,
	esckey_down,
	esckey_right,
	esckey_left,
	esckey_home,
	esckey_insert,
	esckey_delete,
	esckey_end,
	esckey_pgup,
	esckey_pgdn,
	esckey_f1,
	esckey_f2,
	esckey_f3,
	esckey_f4,
	esckey_f5,
	esckey_f6,
	esckey_f7,
	esckey_f8,
	esckey_f9,
	esckey_f10,
	esckey_f11,
	esckey_f12
} esc_key_code;

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <glog.h>

#define TTY_LINE_BUF_SIZE 128
#define MAX_TTY_CMD_FUNC 32
#define USE_TTY_PROMPT

#ifdef USE_TTY_PROMPT
	#define TTY_PROMPT0	"#"
	#define TTY_PROMPT1 "$"
	#define TTY_PROMPT2 "]"
	#define TTY_PROMPT3 ">"
	#define TTY_PROMPT TTY_PROMPT1
#else
	#define TTY_PROMPT ""
#endif

int set_tty_func(const char*cmdstr,void (*func)(const char *));
void tty(void);
extern int direct_escape_key;
//int tty_direct_func(int c);
//void help_tty_msg();
//int tty_prev_func(int c);
void tty_inner_cmd_list();

#ifdef __cplusplus
}
#endif
#endif // __GAU_TTY_H
