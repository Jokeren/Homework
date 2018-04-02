// Header include
#include <determinant.h>
// System include 
#include <stdlib.h>
#include <string.h>
// Local include
#include <kernels/plain.h>
#include <kernels/blas.h>
#include <kernels/simd.h>
#include <utils/log.h>

#define STR(x) #x

#ifdef USE_OPENBLAS
#define FOREACH_FUNC_NAME(macro)             \
  macro(float,  plain, determinant_s_plain)  \
  macro(double, plain, determinant_d_plain)  \
  macro(long long, plain, determinant_ll_plain) \
  macro(float,  simd,  determinant_s_simd)   \
  macro(double, simd,  determinant_d_simd)   \
  macro(long long, simd, determinant_ll_simd) \
  macro(float,  blas,  determinant_s_blas)   \
  macro(double, blas,  determinant_d_blas)   
#else
#define FOREACH_FUNC_NAME(macro)             \
  macro(float,  plain, determinant_s_plain)  \
  macro(double, plain, determinant_d_plain)  \
  macro(long long, plain, determinant_ll_plain) \
  macro(float,  simd,  determinant_s_simd)   \
  macro(double, simd,  determinant_d_simd)   \
  macro(long long, simd, determinant_ll_simd)
#endif


#ifdef PERFORMANCE
#define DETERMINANT_FUNC(type, name, func)          \
type func(size_t N, type *M) {                      \
  type result = func##_kernel(N, M);                \
  return result;                                    \
}
FOREACH_FUNC_NAME(DETERMINANT_FUNC)
#undef DETERMINANT_FUNC
#else
#define DETERMINANT_FUNC(type, name, func)  \
type func(size_t N, type *M) {              \
  if (N == 0) {                             \
    LOG_ERROR(STR(func), "N: %zu", N);      \
  } else {                                  \
    LOG_INFO(STR(func), "N: %zu", N);       \
  }                                         \
  type result = func##_kernel(N, M);        \
  return result;                            \
}
FOREACH_FUNC_NAME(DETERMINANT_FUNC)
#undef DETERMINANT_FUNC
#endif


void *lookup_determinant_func(const char *data_type, const char *kernel_name) {
#define find_determinant_func(type, name, func)                                   \
  if (strcmp(kernel_name, STR(name)) == 0 && strcmp(data_type, STR(type)) == 0) { \
    return (void *)func;                                                          \
  }
  FOREACH_FUNC_NAME(find_determinant_func)
#undef find_determinant_func
  return NULL;
}

#undef FOREACH_FUNC_NAME
