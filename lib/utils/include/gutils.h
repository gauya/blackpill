
/*
 * gutils.h
 *
 *  Created on: 2021. 10. 15.
 *      Author: seu
 */

#ifndef GUTILS_H_
#define GUTILS_H_

#ifndef ABS
#define ABS(x)        ( ((x)<0)?-(x):(x) )
#define MAX(x,y)      ( (x)>(y)?(x):(y) )
#define MIN(x,y)      ( (x)<(y)?(x):(y) )
#endif

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef union {
	uint32_t wD;
	struct {
		uint32_t	BT0:1;
		uint32_t	BT1:1;
		uint32_t	BT2:1;
		uint32_t	BT3:1;
		uint32_t	BT4:1;
		uint32_t	BT5:1;
		uint32_t	BT6:1;
		uint32_t	BT7:1;
		uint32_t	BT8:1;
		uint32_t	BT9:1;
		uint32_t	BT10:1;
		uint32_t	BT11:1;
		uint32_t	BT12:1;
		uint32_t	BT13:1;
		uint32_t	BT14:1;
		uint32_t	BT15:1;
		uint32_t	BT16:1;
		uint32_t	BT17:1;
		uint32_t	BT18:1;
		uint32_t	BT19:1;
		uint32_t	BT20:1;
		uint32_t	BT21:1;
		uint32_t	BT22:1;
		uint32_t	BT23:1;
		uint32_t	BT24:1;
		uint32_t	BT25:1;
		uint32_t	BT26:1;
		uint32_t	BT27:1;
		uint32_t	BT28:1;
		uint32_t	BT29:1;
		uint32_t	BT30:1;
		uint32_t	BT31:1;
	};
} w32BT_t;

int bit_count(uint32_t word);

// max > min
typedef struct {
	double	max; //
	double	min; //
} pg_t;

typedef struct {
	uint32_t cb,n;
	double  v;
} gavg_t;


double topg( pg_t *p, double v );
int toph( pg_t *p, double v );

uint32_t dif_u32(uint32_t s,uint32_t e);
uint32_t dif_u32_limit(uint32_t s,uint32_t e,uint32_t max);

double gavg(gavg_t *k, double nv );
void set_gavg(gavg_t *k, uint8_t no);
double last_gavg(gavg_t *a);

#ifdef __cplusplus
}
#define DEF_GAVG_CALIBRATION 10

class gavg : gavg_t {
public:
	gavg(int cs = DEF_GAVG_CALIBRATION);
	virtual ~gavg();

	inline double get() const { return v; }
	double put( double val );
	void set(uint32_t nb);
	double operator=(double val) { return put(val); }
	inline operator double() const { return get(); }
};

template <typename T,typename N>
struct gavg_tt{
	N cb,n;
	T v; 
};
typedef gavg_tt<double,uint32_t> gavgdu_t;

template<typename T, typename N>
class gtg : public gavg_tt<T,N> {
	//gavg_tt<T,N> _d;
protected:
	using gavg_tt<T,N>::v;
	using gavg_tt<T,N>::cb;
	using gavg_tt<T,N>::n;
public:
	gtg(N cs = DEF_GAVG_CALIBRATION);
	virtual ~gtg();

	inline T get() const { return this->v; }
	T put( T val );
	void set(N nb);
	T operator=(T val) { return put(val); }
	inline operator T() const { return get(); }
};
// gavg_tt<int,uint8_t> gr;

class gam {
private:
	gavg_t *_gs = 0;
	int _len = 0;
public:
	gam(int no, int cs[]);
	virtual ~gam();

	double get(int id = 0);
	double put( double v );
	double set(int id,uint32_t n);
};

#else // c

#ifndef bool
typedef enum {
	false = 0,
	true = 1
} bool;
#endif

#endif // cplusplus

#endif /* GUTILS_H_ */
