
/*
 * gsort.h
 *
 *  Created on: Nov 3, 2021
 *      Author: seu
 */

#ifndef GSORT_H_
#define GSORT_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
// sort

#define SORT_BUBBLE		1
#define SORT_SELECTION	2
#define SORT_INSERTION	3

template <typename T>
int comp(T a, T b);

// n x (n-1)
template <typename T>
int gsort_0(T *v, size_t sz);

// bubble
template <typename T>
int gsort_1(T *v, size_t sz);

// selection
template <typename T>
int gsort_2(T *v, size_t sz);

// insertion
template <typename T>
int gsort_3(T *v, size_t sz);

/*
 * STDLIB.H
 * qsort( void*, size_t len, size_t size, int (*compf)( const void*, const void * );
 * len = sizeof(arr)/sizeof(arr[0])
 * size = sizeof(arr[0])
 *
 */

template <typename T>
int gsort(T *v, size_t sz,int type=SORT_INSERTION);

#endif // cplusplus

#endif /* GSORT_H_ */
