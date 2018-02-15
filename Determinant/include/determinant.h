#ifndef _INCLUDE_DETERMINANT_H_
#define _INCLUDE_DETERMINANT_H_

#include <stddef.h>

typedef float (*determinant_s_fn_t)(size_t N, float *M);

typedef double (*determinant_d_fn_t)(size_t N, double *M);

typedef long long (*determinant_ll_fn_t)(size_t N, long long *M);

extern determinant_s_fn_t lookup_determinant_s(const char *kernel_name);

extern determinant_d_fn_t lookup_determinant_d(const char *kernel_name);

extern determinant_ll_fn_t lookup_determinant_ll(const char *kernel_name);

#endif  // _INCLUDE_DETERMINANT_H_
