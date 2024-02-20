
/*
 * printf
 *
 * 2014.11.10 ~ , 
 * hwang.chiyeong@gmail.com
 *
 * 0.01 
 * 
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>

#include "gprintf.h"

static int __default_output = 0;

int default_output(int nr) {
	if( nr == -1 ) {
		return __default_output;
	}
	__default_output = nr;
	return __default_output;
}

typedef struct  
{
	int	mode;	// step
	const char *fmtp;
	const char *base_fmtp;
	char *bufp;
	char *base_bufp;
	int	cp;	// indicate bufp end point
	int	bsize;

	int	ch;		// dscufxob
	int	sign;
	int	lflg;		// long 'l' option 
	int	max;  	// width restrict maximun at string, float point 
	int	width;	// width

	char	fill;
	char	dill;
	char	slice;
	char	upstr;	// hex large

	int	argc;		// effective argments 
} _st_fmt_t;

void _st_clear(_st_fmt_t *f)
{
	f->mode = f->ch = f->sign = f->lflg = f->max = f->width = 0;
	f->slice = f->upstr = '\0';
}

void _st_init(_st_fmt_t *f, const char* fmt, char *buf, int sz )
{
	memset(f,0,sizeof(_st_fmt_t));

	f->base_fmtp = f->fmtp = fmt;
	f->base_bufp = f->bufp = buf;
	f->bsize = sz;
}

static void debug(_st_fmt_t* f,const char* msg ) {
#ifdef GPRINT_DEBUG
	printf("%s[m=%d,'%c',slmwa(%d,%d,%d,%d),argc=%d Op[%s],Np(%s)--\n", msg,
		f->mode,f->ch,
		f->sign,f->lflg,f->max,f->width,f->argc,
		f->base_bufp,f->fmtp);
#endif
}

typedef enum 
{
	_ERR = -1,
	_END = 0,
	_STR = 1, 
	_NUM = 2, // 0 ~ 9
	_PAS = 3, // dscufxobXB 
	_SIN = 4, // -, +
	_PST = 5, // %
	_LMT = 6, // .
	_LGE = 7, // l
	_NOR = 9
} _rt_type;

static const char *__chars = "sdcfuxobXn";
static const char __d__[] = "0123456789ABCDEF";
static const char __ld__[] = "0123456789abcdef";

/*
  %) .n maximum string or float dot below 
  %) nnn digit -> _get_digit
  %) not specified float then max = 6
  %) not support long long

mode ( disclude -- #, p
  	@ = d, c, x, X, b, B, f
  	$ = ., l
	# = $, @
	^ = -, +

	0 : normal  all                 <-- start   || mode=99 || (mode=1 && detect %)
	1 : expect %, l, ^, 0-9, @,     <-- mode=0  && detect %
	2 : expect l, 0-9, @,           <-- mode=1  && detect ^
	3 : expect 0-9, #               <-- mode=2  && detect 0-9
	5 : expect 0-9, l, f            <-- mode=4  && detect 0-9 && set max
	6 : expect f,s                  <-- mode>2  && detect l && set max
	7 : expect 0-9                  <-- mode>1  && detect .
	8 : expect @                    <-- mode>0  && detect l
	99: after  @                    <-- va_arg 

0 -> 1
1 -> 0
     2
	  7
	  8
	  3
2 -> 8
     3

*/
//#define abs(v) ( ((v)<0)? -(v) : (v) )

static int
_fmt_oct( _st_fmt_t *f, int value, int rad)
{
//	int v = value,sft,tv,i,k,sz,cnt;
	int sft,tv,i,k,cnt;
	uint32_t chk;
	char buf[33];
	if( f->fill == '\0' ) f->fill = ' ';
#if 0	
	char dill = (f->dill)? f->dill : ' ';
	char fill = f->fill;
#endif	

	switch(rad) {
	case 2:
		chk = 0x1; cnt = 32; sft = 1;
		break;
	case 8:
		chk = 0x7; cnt = 11; sft = 3;
		break;
	case 16:
		chk = 0xf; cnt =  8; sft = 4;
		break;
	default:
		return -1;
	}

	for( i=0,k=0; i<cnt; i++,k++ )
	{
#if 0	
		if( (i>0 && f->slice>0 ) && (i % (int)f->slice) == 0 ) {
			*(f->bufp)++ = dill;	// dill may be ':' or space   00:3c:5f:24
		}
#endif		
		if( value ) {
			tv = (value & chk);
			value >>= sft;
			buf[k] = (f->upstr)? __d__[tv] : __ld__[tv];
		} else { 
			if( k==0 ) buf[k++] = '0';
			break;
		}
	}
	if( f->width < k ) f->width = k;

	for( i=0; (f->width - k)>0; i++,f->width--)
		*(f->bufp)++ = f->fill;

	do { *(f->bufp)++ = buf[--k]; } while(k);
	*(f->bufp) = '\0';

	return 0;
}

static int 
_fmt_str( _st_fmt_t *f, const char* str)
{
	int cols=0,len = strlen(str);
	
	if( f->width == 0 ) f->width = len;
	else if( f->width < len ) len = f->width;
	if( f->max >0 && f->max < len ) len = f->max;

	// fill space
	for( ; f->sign && f->width>len; f->width--,cols++ ) *(f->bufp)++ = ' ';

	for( ;(len--);cols++ ) 
	{
		*(f->bufp)++ = *str++;
	}

	for( ; !f->sign && f->width>len; f->width--,cols++ ) *(f->bufp)++ = ' ';
	*(f->bufp) = '\0';

	return cols;
}

static int 
_fmt_char( _st_fmt_t *f, char ch)
{
	char buf[]=".";
	buf[0] = ch;

	return _fmt_str(f,buf);
}

static int
_fmt_long( _st_fmt_t *f, long avalue)  // ,int rad
{
	long v,value = (avalue < 0 )? -avalue : avalue;
	int len,cp = 0;
	char buf[20];
	
	do {
		v = value % 10;	// rad argument 
		value /= 10;
		buf[cp++] = ( v + '0' );
	} while( value>0 );

	len = cp;
	if( f->sign == 0 && f->width > cp ) {
		if( f->fill == '\0' ) f->fill = ' ';
		while( f->width-- > cp ) { *(f->bufp)++ = f->fill; len++; }
	}

	if( avalue < 0 ) {
		len++;
		*(f->bufp)++ = '-';
	}
	while( cp-- ) 
	{
		*(f->bufp)++ = buf[cp];
	}

	cp = len;
	if( f->sign != 0 && f->width > cp ) {
		if( f->fill == '\0' ) f->fill = ' ';
		while( f->width-- > cp ) { *(f->bufp)++ = f->fill; len++; }
	}
	*(f->bufp) = '\0';
	
	return len;
}

static uint32_t __p[] = { 1L, 10L,100L,1000L,10000L,100000L,1000000L,10000000L,100000000L,1000000000L };

static long _p( unsigned long a, int r )
{
#ifdef _USE_VRADIAN
	long val = 1;
	do {
		val *= a;
	} while( --r );
	
	return val;
#else
	assert( r>=0 && r<=9 );
	return __p[r];
#endif
}

#define _LMT_FLOAT_DOTLEN 6

static int
_fmt_float( _st_fmt_t *f, double value)
{
	if( f->max == 0 || f->max >_LMT_FLOAT_DOTLEN ) 
		f->max = _LMT_FLOAT_DOTLEN;

	unsigned char col=0,lcol=0;
	long ivalue = (long)value;
	double dvalue = value - (double)ivalue;
	
debug(f,"FLOAT");

	if(ivalue < 0) f->width -= 1;
	if( f->width < f->max )
		f->width = f->max + 2;

	if( f->max > 0 ) {
		f->width -= (f->max + 1);
	}

	lcol = _fmt_long( f,ivalue );
debug(f,"1");
	
	*(f->bufp)++ = '.';
	col = lcol + 1;
	
	if( dvalue<0 ) dvalue = -dvalue;
	long v,pv = _p(10,f->max);
	v = (long)(dvalue * (double)pv);
	
	f->fill = '0';
	f->width = f->max;
	lcol = _fmt_long(f,v);
debug(f,"2");

	return (f->sign + col + lcol);
}
#undef _LMT_FLOAT_DOTLEN
// return >= 0, else occur error

static int
_parse_digit( const char** str, char *tp )
{
	int  cp=0;

	for(cp=0; (cp<4 && **str) ;cp++) {
		if( **str < '0' || **str > '9' ) break;
		*tp++ = *(*str)++;
	}
	if( cp == 0 ) {
		if( **str == '\0' )
			return _END;
		else
			*tp++ = '0';  // no readable, default fill one '0'
	}
	*tp = '\0';

	return _NUM;
}

static int _next_ch( const char** str );

static int 
_parse_str( const char **str, char *tp )
{
	int ch,cnt=0;
	while( (ch = _next_ch(str)) > _END )
	{
		if( ch == '%' ) {
			(*str)--;
			break;
		}
		cnt++;
		*tp++ = (char)ch;
	}
	*tp = '\0';

	return cnt;
}

// return token type
static int
_typeis( char ch ) 
{
	if( ch == '\0' )
		return _END;
	if( ch == '%' ) 
		return _PST;
	if( ch >= '0' && ch <= '9' ) 
		return _NUM;
	if( ch == '-' || ch == '+' ) 
		return _SIN;
	if( ch == '.' )
		return _LMT;
	if( ch == 'l' ) 
		return _LGE;
	if( strchr( __chars,ch ) )
		return _PAS;
	return _NOR;
}

static int
_next_ch( const char** str )
{
	int ch = *(*str)++;
	if( ch == '\0' ) {
		(*str)--;
		return -1; // end
	}
	
	if( ch == 27 ) {
		ch =  _next_ch(str);
		if( ch == -1 )
			return -1;
		switch(ch) {
		case 'r':	ch = '\r'; break;
		case 'n':	ch = '\n'; break;
		case 't':	ch = '\t'; break;
		case 'f':	ch = '\f'; break;
		case '0':	ch = '\0'; break;
		default:
			;
		}
	}
	
	return (int)ch;
}

static int
_next_tok( _st_fmt_t *f, char *tmpb )
{
	*tmpb='\0';

	int ch= _next_ch(&f->fmtp); 
	if( ch == -1 ) {
		f->fmtp--;
		return _END;  // end
	}

debug(f,"D");
	f->ch = *tmpb = (char)ch;
	tmpb[1] = '\0';
	if( f->mode == 0 ) {
		if( ch=='%' ) 
			return _PST;
		else {
			f->fmtp--;
			char *ib = tmpb;
debug(f,"$");
			_parse_str( &f->fmtp,tmpb );
debug(f,"%");
			while( *ib ) *f->bufp++ = *ib++;
			*f->bufp = '\0';
			
			return _STR;
		}
	}
	if( ch >= '0' && ch <= '9' ) {
		f->fmtp--;
		_parse_digit( &f->fmtp,tmpb );
		return _NUM;
	}
	if( ch == '-' || ch == '+' ) {
		return _SIN;	// next_type should be _NUM
	}
	if( ch == '.' ) {
		return _LMT;	// next_type should be _NUM
	}
	if( ch == 'l' ) {
		return _LGE;	// next_type should be _PAS
	}
	if( strchr( __chars, ch ) ) {
		return _PAS;
	}
	
	return _ERR;	// error 
}

typedef int (*__mode_expectf)( const char ch );

__attribute__((weak))int gwputc(int ch,int where) {
	return 0;
}

__attribute__((weak))int gwputs(const char *str,int where) {
	if(where == 0) {
		return puts(str);
	}
	int l;
	for(l=0; *str; str++,l++) {
		if(gwputc(*str,where) < 1) return -1;
	}
	return l;
}

__attribute__((weak))int gwwrite(const char *str, size_t len, int where ) {
	return 0;
}

__attribute__((weak))int gputc(int v) {
	return gwputc(v,__default_output);
}

__attribute__((weak))int gputs(const char *str) {
	return gwputs(str,__default_output);
}

__attribute__((weak))int gwrite(const char *str, size_t len) {
	return gwwrite(str,len,__default_output);
}

#include <stdlib.h>
int gvbprintf( char *buf, int sz, const char* fmt, va_list ap)
{
	int type, ntype;
	char ibuf[_MAX_GPFMTBUF+1], *s;
	char *ib=ibuf;
	_st_fmt_t f;

	int rad, v, c, err=0;
	double d;

	_st_init(&f,fmt,buf,sz);

debug(&f,"start ");		
	while(( type = _next_tok(&f, ibuf)) > _END ) 
	{
		ntype = _typeis( *(f.fmtp) );
debug(&f,"->"); 
		switch(type) {
		case _STR:
			_st_clear(&f);
			if( ntype != _PST && ntype != _END ) {
				err = 1; break;
			}
			break;
		case _NUM:
			f.mode = (f.max != -1)? 4: 6;
			v = atoi(ib);
			if( f.max < 0 ) f.max = v;
			else f.width = v;
			if(ib[0]=='0') f.fill = '0';
			break;	
		case _PAS:
			f.ch = *ib;
			f.mode = 0;
			f.argc++;
			
//			f.fill = 0;
			switch( f.ch ) 
			{
			case 's':
				s = va_arg(ap, char *);
				_fmt_str(&f,s);
				break;
			case 'd': case 'u':
				v = va_arg(ap, int);
				_fmt_long(&f,v);
				break;
			case 'c':
				c = va_arg(ap, int);
				_fmt_char(&f,c);
				break;
			case 'f':
				d = va_arg(ap, double);
				_fmt_float(&f,d);
				break;
			case 'x': 
			case 'X': 
			case 'o': 
			case 'b':
				v = va_arg(ap, int);
				rad = (f.ch == 'x' || f.ch == 'X')? 16:(f.ch == 'o')? 8:2;
				f.upstr = ( f.ch == 'X')? 1:0;
				_fmt_oct(&f,v,rad);
				break;
			default:
				;  // error? else ignore 
				f.argc--;
			}
			f.argc++;
			break;	
		case _LMT:
			f.mode = 5;
			if( ntype != _NUM ) { 
				err = 1; break;
			}
			f.max = -1; // when max == -1 next number is set to max
			if( ntype != _NUM ) {
				// error 
			}
			break;	
		case _LGE:
			f.mode = 7;
			if( ntype != _PAS ) {
				err = 1; break;
			}
			if( ntype != _PAS ) {
				// error
			}
			f.lflg = 1;
			break;	
		case _SIN:
			f.mode = 2;
			if( ntype != _NUM ) {
				err = 1; break;
			}
			if( *ib == '-' ) f.sign = -1;
			if( ntype != _NUM ) {
				// error
			}
			break;	
		case _PST:
			if( f.mode == 1 ) {
				f.mode = 0;
			} else 
			if( f.mode == 0 ) {
			// initial f
				_st_clear(&f);
				f.mode = 1;
			} else {
				// error;
			}
			f.fill = 0;
			break;	
		default:
			break;
		}
		if( err ) break;
		ib = ibuf;
	}
debug(&f,"<<");	
	va_end(ap);

	return f.argc;
}

int gprintf( const char* fmt, ...)
{
	char buf[_MAX_GPBUF_SZ+1];
	va_list ap;

	va_start(ap, fmt);
	int i = gvbprintf(buf, _MAX_GPBUF_SZ, fmt, ap);
	va_end(ap);
	
	gputs(buf);
	return i;
}

int gvprintf( const char* fmt, va_list ap)
{
	char buf[_MAX_GPBUF_SZ+1];

	int re = gvbprintf(buf,_MAX_GPBUF_SZ,fmt,ap);
	gputs(buf);

	return re;
}

int gsnprintf( char *buf, int sz, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	
	int re = gvbprintf(buf, sz, fmt, ap);
	va_end(ap);
	
	return re;
}

#if 0
/*
 * int vsnprintf(char *pStr, size_t length, const char *pFormat, va_list ap)
 * %-7.3f, %05d, %7ld, %-15s, x,X,p,c,b,o,d,
 * 
 */
int gprintf_test(char *buf,int blen)
{
	int v=-24353476;
	double d = -735234.2424433;

	return gsnprintf(buf,blen,"Hi. %-15s,%15o, %b, %15.3f,%-12d bye!!","Hello!!\n",v,v,d,v);
}
#endif
