
/*
 * gfifo.cpp
 *
 *  Created on: 2021. 10. 13.
 *      Author: seu
 */

#include <malloc.h>
#include <string.h>
#include <gfifo.h>

static int _fifo_isvalid(gfifo_t* f)
{
	return (f && (f->size > 1) && f->data && (f->cp < f->size) && (f->np < f->size) )? 1 : 0;
}

static int _fifo_isfull(gfifo_t* f)
{
	return (((f->cp + 1) == f->np) || (f->np == 0 && (f->cp == (f->size - 1))) );
}

static int _fifo_isempty(gfifo_t* f)
{
	return (f->cp == f->np);
}

int fifo_clear(gfifo_t* f)
{
	if( !_fifo_isvalid(f) )
		return -1;
	f->cp = f->np;

	return 0;
}

int fifo_getfree(gfifo_t* f)
{
	if( !_fifo_isvalid(f) ) return -1;

	if( f->cp <= f->np )
	{
		return (int)(f->np - f->cp);
	} else
	{
		return (int)(f->size - f->cp + f->np);
	}
}

static int _fifo_push(gfifo_t* f,int ch)
{
	if(f->lockfunc) f->lockfunc(1);

	if( _fifo_isfull(f) ) {
		if( !f->flag ) {
			ch = -1; //
		}
		if( f->flag == FIFO_DISTROY_NEW ) {
			ch = 0;
			f->overflow = 1;
		}
		if( f->flag == FIFO_WAIT ) {
			//while( fifo_isfull(f) );
			ch = -2;
		}
		if( f->flag == FIFO_DISTROY_OLD ) {
			if( ++(f->np) >= f->size ) f->np = 0; // old data destroy
			f->overflow = 1;
		}
	} else {
		f->data[f->cp] = (uint8_t)ch;
		if( ++(f->cp) >= f->size ) f->cp = 0;
	}

	if(f->lockfunc) f->lockfunc(0);

	return ch;
}

static int _fifo_pop(gfifo_t* f)
{
	int ch = 0;

	if(f->lockfunc) f->lockfunc(1);

	if( ! _fifo_isempty(f) )
	{
		ch = (int)f->data[f->np];
		if( ++(f->np) >= f->size ) f->np = 0;
	}
	f->overflow = 0;

	if(f->lockfunc) f->lockfunc(0);

	return ch;
}

int fifo_puts(gfifo_t* f,const char *buf)
{
	int i = 0,ch;
	if( _fifo_isvalid(f) == 0 )
		return -1;

	for( i = 0; buf[i]; i++ ) {
		ch = _fifo_push(f,(int)buf[i]);
		if( ch <= 0 || buf[i] == '\n' ) break;
	}

	return i;
}

int fifo_gets(gfifo_t* f,char *buf, uint16_t len)
{
	int ret = 0;
	if( _fifo_isvalid(f) == 0 || len < 2 )
		return -1;

	len -= 1;
	for( ret = 0; ret < len; ret++ ) {
		int val = _fifo_pop(f);
		if( val <= 0 ) break;
		buf[ret] = val;
		if( val == '\n' ) break;
	}
	buf[ret] = 0;

	return ret;
}


void fifo_init(gfifo_t* f, int bsize, int flag,void (*lockf)(int))
{
	memset(f->data,0,bsize);

	f->size = bsize;  // size << 1;
	f->cp = f->np = 0;
	f->flag = (flag == FIFO_WAIT || flag == FIFO_DISTROY_NEW || flag == FIFO_DISTROY_OLD )? flag : FIFO_DEF_MODE;
	f->overflow = 0;
	f->lockfunc = lockf;
}


int fifo_set(gfifo_t* f,uint8_t *buf, int size, int flag)
{
	if( !f ) return -1;
	if( buf ) {
		f->data = buf;
		f->alloc = 0;
	} else {
		if( size < 2 || size > FIFO_MAX_SIZE ) size = FIFO_MAX_SIZE;
		f->data = (uint8_t *)malloc(size+1);
		f->alloc = 1;
	}

	fifo_init(f, size, flag, 0 );

	return (int)size;
}

int fifo_reset(gfifo_t* f)
{
	if( ! f )
		return -1;
	if( f->alloc ) {
		free(f->data);
	}
	f->data = 0;
	f->size = f->flag = 0;
	f->cp = f->np = 0;
	f->overflow = 0;
	f->lockfunc = 0;

	return 1;
}

int fifo_putc(gfifo_t* f, int ch)
{
	if( !_fifo_isvalid(f) )
		return -1;

	return _fifo_push(f,ch);
}

int fifo_getc(gfifo_t* f)
{
	if( !_fifo_isvalid(f) )
		return -1;

	return _fifo_pop(f);
}

int fifo_write(gfifo_t* f,const uint8_t *buf, uint16_t len)
{
	if( ! _fifo_isvalid(f) )
		return -1;

	for( int i=0; i< len; i++ ) {
		if ( _fifo_push(f, buf[i]) <= 0 )
			return i;
	}

	return len;
}

int fifo_read(gfifo_t* f,uint8_t *buf, uint16_t len)
{
	if( ! _fifo_isvalid(f) )
		return -1;

	int ch;
	for( int i=0; i < len; i++) {
		if( (ch = _fifo_pop(f)) <= 0 )
			return i;
		buf[i] = (uint8_t)ch;
	}

	return len;
}

void fifo_setlock(gfifo_t* f, void (*lf)(int))
{
	if( ! _fifo_isvalid(f) )
		return;
	f->lockfunc = lf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int gfifo::isvalid()
{
	return _fifo_isvalid((gfifo_t*)this);
//	return ((this->size > 1) && this->data && (this->cp < this->size) && (this->np < this->size) )? 1 : 0;
}

int gfifo::isfull()
{
	return (((this->cp + 1) == this->np) || (this->np == 0 && (this->cp == (this->size - 1))) );
}

int gfifo::isempty()
{
	return (this->cp == this->np);
}

int gfifo::clear()
{
	if( !isvalid() )
		return -1;
	this->cp = this->np;

	return 0;
}

int gfifo::getfree()
{
	if( !isvalid() ) return -1;

	if( this->cp <= this->np )
	{
		return (int)(this->np - this->cp);
	} else
	{
		return (int)(this->size - this->cp + this->np);
	}
}

int gfifo::push(int ch)
{
	if(this->lockfunc) this->lockfunc(1);

	if( isfull() ) {
		if( !this->flag ) {
			ch = -1; //
		}
		if( this->flag == FIFO_DISTROY_NEW ) {
			ch = 0;
			this->overflow = 1;
		}
		if( this->flag == FIFO_WAIT ) {
			//while( fifo_isfull(f) );
			ch = -2;
		}
		if( this->flag == FIFO_DISTROY_OLD ) {
			if( ++(this->np) >= this->size ) this->np = 0; // old data destroy
			this->overflow = 1;
		}
	} else {
		this->data[this->cp] = (uint8_t)ch;
		if( ++(this->cp) >= this->size ) this->cp = 0;
	}

	if(this->lockfunc) this->lockfunc(0);

	return ch;
}

int gfifo::pop()
{
	int ch = 0;

	if(this->lockfunc) this->lockfunc(1);

	if( ! isempty() )
	{
		ch = (int)this->data[this->np];
		if( ++(this->np) >= this->size ) this->np = 0;
	}
	this->overflow = 0;

	if(this->lockfunc) this->lockfunc(0);

	return ch;
}

gfifo::operator gfifo_t *() { // dynamic_cast<Student *>(Person[i]);
	return dynamic_cast<gfifo_t*>(this);
}

#if 0
// string fifo

int fifo_puts(const char *buf)
{
	int ret = 0;
	if( _fifo_isvalid(f) == 0 )
		return -1;

	for( int i = 0; i < len && buf[i]; i++) {
		ret = _fifo_push(f,(int)buf[i]);
		if( ret <= 0 || buf[i] == '\n' ) break;
	}

	return ret;
}

int fifo_gets(char *buf, uint16_t len)
{
	int ret = 0;
	if( _fifo_isvalid(f) == 0 )
		return -1;

	for( ret = 0; ret < len; ) {
		int val = _fifo_pop(f);
		if( val < 0 ) return ret;
		buf[ret++] = (uint8_t)val;
		buf[ret] = 0;
		if( val == '\n' ) break;
	}

	return ret;
}
#endif

#include <stdlib.h>
#include <string.h>

void gfifo::init(int bsize, int flag,void (*lockf)(int))
{
	memset(this->data,0,bsize);

	this->size = bsize;  // size << 1;
	this->cp = this->np = 0;
	this->flag = (flag == FIFO_WAIT || flag == FIFO_DISTROY_NEW || flag == FIFO_DISTROY_OLD )? flag : FIFO_DEF_MODE;
	this->overflow = 0;
	this->lockfunc = lockf;
}

gfifo::gfifo(int bsize, int flag,void (*lockf)(int))
{
	if( bsize < 2 || bsize > FIFO_MAX_SIZE ) bsize = FIFO_MAX_SIZE;
	this->data = (uint8_t *)malloc(bsize+1);
	alloc = 1;
	init(bsize,flag,lockf);
}

gfifo::gfifo(uint8_t *buf, int bsize, int ftype, void (*lockf)(int)) {
	if( buf ) {
		data = buf; // extern buffer size may be larger then FIFO_MAX_SIZE
		alloc = 0;
	} else {        // assert ?
		if( bsize < 2 || bsize > FIFO_MAX_SIZE ) bsize = FIFO_MAX_SIZE;
		data = (uint8_t *)malloc(bsize+1);
		alloc = 1;
	}
	init(bsize, ftype, lockf);
}

gfifo::~gfifo()
{
	if( alloc ) {
		free(this->data);
		this->data = 0;
	} else {
		this->data[0] = 0;
	}

	this->flags = 0;
	this->cp = this->np = 0;
	this->lockfunc = 0;
}

int gfifo::putc(int ch)
{
	if( !isvalid() )
		return -1;

	return push(ch);
}

int gfifo::getc()
{
	if( !isvalid() )
		return -1;

	return pop();
}

int gfifo::write(const uint8_t *buf, uint16_t len)
{
	if( ! isvalid() )
		return -1;

	for( int i=0; i< len; i++ ) {
		if ( push(buf[i]) <= 0 )
			return i;
	}

	return len;
}

int gfifo::read(uint8_t *buf, uint16_t len)
{
	if( ! isvalid() )
		return -1;

	int ch;
	for( int i=0; i < len; i++) {
		if( (ch = pop()) <= 0 )
			return i;
		buf[i] = (uint8_t)ch;
	}

	return len;
}
