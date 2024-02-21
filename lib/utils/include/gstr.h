
/*
 * gstr.h
 *
 *  Created on: Dec 14, 2021
 *      Author: seu
 */

#ifndef GSTR_H_
#define GSTR_H_

#include <string.h>
#include <cstdint>

typedef enum {
	eEND = 0,
	eALPA,     // constant, name
	eNUM,      // constant, 			
	eOP,       // 
	eDIL,
	eSPACE,
	eESC,
	eBLK,
	eERR = -1
} etoken_type;

typedef enum {
	eBLOCK=0,
	eLINE_COMMENT,
	eCOMMENT,
	eWIDE_CONSTANT,
	eCHAR_CONSTANT,
	eNOTBLOCK = -1
} eblock_type;

typedef enum {
	eARITHMETIC, //arithmetic  - +, -, *, /, %, ++, --
	eRELETIONAL, //relational  - ==, !=, >, <, >=, <=
	eLOGICAL,    //logical     - &&, ||, !
	eASSIGNMENT, //Assignment  - =, +=, -=, *=, /=, %=, <<=, >>=, &=, ^=, |=
	eBITWISE,    //Bitwise     - |, &, ^, ~, <<, >>
	eSPEICIAL,   //Special     - &,*,sizeof
	eCONDITION  //Conditional - ?:
//	ePOSTFIX     //            - ., ->, :
} eop_type;

typedef struct {
	eblock_type etype;
	const char *start;
	const char *end;
} block_type_t;

typedef struct {
	block_type_t *bt;
	int len;
} block_info_t;

#if 0
/*
Postfix	() [] -> . ++ - -	Left to right
Unary	+ - ! ~ ++ - - (type)* & sizeof	Right to left
*/
typedef struct {
   eop_type etype;
   const char*opstr;
} eop_type_t;
#endif

typedef struct {
	char *buf;
	size_t bsize;
	int len;
	int etype; // token type
	int subtype;  // sub type
	int step;
} ptoken_t;

#ifdef __cplusplus
extern "C" {
#endif

extern const char __default_white_space[];
extern const char __default_delimiter[];

void touppers(char *str) ;
void tolowers(char *str) ;
const char* whitespace_skip(const char *b);
const char *str_skip(const char *b,const char *s);
int strchri(const char *s,int ch) ;
int cmpstr(const char *s,const char *d) ;

int instrs(const char *s,const char **dest) ;
int stoi(const char *s) ;
int64_t stol(const char *s) ;
uint32_t stob(const char *s) ;
uint32_t stoo(const char *s) ;
uint32_t stox(const char *s) ;
double stof(const char *s) ;
char *strnzcpy(char *s1, const char *s2, size_t n);
bool is_block_string(const char *str) ;
int istoktype(const char *s) ;
block_type_t *get_block_type(int idx) ;

block_type_t *is_start_block(const char *str) ;
block_type_t *is_end_block(const char *str) ;
block_type_t *get_block_type(int) ;
const char *gettoken(const char *s, char *buf,int dilimeter,const char *space) ;
const char *get_token(const char *s, char *buf, size_t blen) ;
block_info_t get_block(const char **str, char *buf, size_t bsize, int step) ;
const char *next_token(const char *s, ptoken_t *p) ;
// return token_type
const char *get_parse(const char* s, char *buf, size_t blen,int step) ;
int parse(const char *s,int step) ;

/////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}

#include <stdexcept>
using namespace std;

class gstr {
private:
	char *_str;
	uint32_t _len;
	uint32_t _bsize;

public:
	gstr();
	gstr(const char *s);
	gstr(gstr &g);
	virtual ~gstr();

	inline uint32_t len() const { return _len; };
	operator const char*() const;
	gstr& operator=(gstr &);
	gstr& operator=(const char*);
	bool operator==(gstr &) const;
	bool operator==(const char*) const;
	char& at(int);
	char& operator [](int);
	int comp (gstr&); // -1, 0, 1
	int comp (const char *); // -1, 0, 1
	gstr& append(gstr&);
	gstr& append(const char*);
	gstr& operator += (gstr&);
	gstr& operator += (const char*);
	friend gstr operator + (gstr&,gstr&);
	friend gstr operator + (gstr&,const char*);
	gstr& operator <<= (gstr&);
	gstr& operator <<= (const char*);
	void lstrip(const char* d=0);
	void rstrip(const char *d=0);
	gstr& cut(int,int); // from to
	gstr& remove(const char*);
	gstr& remove_all(const char*);
	gstr& replace(const char*, const char* to=0);
	gstr& replace_all(const char*, const char* to=0);
	gstr& lower();
	gstr& upper();
};

class gtoken : public gstr {
public:
public:
	gtoken();
	gtoken(const char *str);
	virtual ~gtoken();
};

#endif // __cplusplus
#endif /* GSTR_H_ */
