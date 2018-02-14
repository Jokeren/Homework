#ifndef _INCLUDE_DETERMINANT_H_
#define _INCLUDE_DETERMINANT_H_

#include <stddef.h>

typedef float (*determinant_s_fn_t)(size_t N, const float *M);

typedef double (*determinant_d_fn_t)(size_t N, const double *M);

extern determinant_s_fn_t lookup_determinant_s(const char *kernel_name);

extern determinant_d_fn_t lookup_determinant_d(const char *kernel_name);

#endif  // _INCLUDE_DETERMINANT_H_
