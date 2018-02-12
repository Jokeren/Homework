#ifndef _INCLUDE_DETERMINANT_H_
#define _INCLUDE_DETERMINANT_H_

#include <stddef.h>

typedef float (*determinant_s_fn_t)(size_t N, float *M);

extern determinant_s_fn_t lookup_determinant_s(char *kernel_name);

#endif  // _INCLUDE_DETERMINANT_H_