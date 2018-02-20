#ifndef _INCLUDE_DETERMINANT_H_
#define _INCLUDE_DETERMINANT_H_

#include <stddef.h>

typedef float (*determinant_s_fn_t)(size_t N, float *M);

typedef double (*determinant_d_fn_t)(size_t N, double *M);

typedef long long (*determinant_ll_fn_t)(size_t N, long long *M);

extern void *lookup_determinant_func(const char *data_type, const char *kernel_name);

#endif  // _INCLUDE_DETERMINANT_H_
