
/*
 * gfifo.h
 *
 *  Created on: 2021. 10. 13.
 *      Author: seu
 */

#ifndef GFIFO_H_
#define GFIFO_H_

#define FIFO_ALLOC	0

#define	FIFO_WAIT	1
#define	FIFO_DISTROY_NEW 0
#define	FIFO_DISTROY_OLD 2
#define FIFO_DEF_MODE	FIFO_DISTROY_NEW

#define FIFO_TINY_MODE	0

#if (FIFO_TINY_MODE == 0)
	#define FIFO_MAX_SIZE 255
#else
	#define FIFO_MAX_SIZE 4096
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
#if (FIFO_TINY_MODE == 1)
	uint8_t	size;
	union {
		uint8_t flags;
	struct {
		uint8_t reserved : 3;
		uint8_t alloc    : 1;
		uint8_t lock     : 1;
		uint8_t	overflow : 1;  // overflow
		uint8_t	flag     : 2;  // discard old/new/wait
	};
	};
	uint8_t	cp;		// write and move
	uint8_t	np;		// not empty then read and move
#else
	union {
	struct {
		uint16_t	reserved     : 11;
		uint16_t	alloc    : 1;
		uint16_t	lock     : 1;
		uint16_t	overflow : 1;
		uint16_t	flag     : 2;  // discard old/new/wait,
	};
		uint16_t flags;
	};
	uint16_t	size;
	uint16_t	cp;		// write and move
	uint16_t	np;		// not empty then read and move
#endif

	uint8_t	*data;
	void (*lockfunc)(int lu);
} gfifo_t;

int fifo_set(gfifo_t* f, uint8_t *buf, int size, int flag);
int fifo_reset(gfifo_t* f);
void fifo_setlock(gfifo_t* f, void(*l)(int));

/*
 * to static
extern void fifo_init(gfifo_t* f, int size, int flag);
extern int fifo_isfull(gfifo_t* f);
extern int fifo_isempty(gfifo_t* f);
extern int fifo_isvalid(gfifo_t* f);
*/
int fifo_clear(gfifo_t* f);
int fifo_getfree(gfifo_t* f);

int fifo_getc(gfifo_t* f);
int fifo_putc(gfifo_t* f, int ch);
int fifo_read(gfifo_t* f,uint8_t *buf, uint16_t len);
int fifo_write(gfifo_t* f,const uint8_t *buf, uint16_t len);
int fifo_puts(gfifo_t* f,const char *buf);
int fifo_gets(gfifo_t* f,char *buf, uint16_t len);

#ifdef __cplusplus
 }
#endif

#ifdef __cplusplus

class gfifo : public gfifo_t {
private:
	int push(int ch);
	int pop();
protected:
	int isfull();
	int isempty();
	int isvalid();
	void init(int bsize, int ftype=0, void (*lockf)(int)=0);
public:
	gfifo();
	gfifo(int bsize, int ftype=0, void (*lockf)(int)=0);
	gfifo(uint8_t *buf, int bsize, int ftype=0, void (*lockf)(int)=0);
	virtual ~gfifo();

	int clear();
	int getfree();
	int putc(int ch);
	int getc();
	int write(const uint8_t *buf, uint16_t len);
	int read(uint8_t *buf, uint16_t len);

	void set_lock_function(void (*f)(int));
	operator gfifo_t *(); // dynamic_cast<Student *>(Person[i]);
};

#endif //__cplusplus

#endif /* GFIFO_H_ */
