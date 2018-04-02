#include <stdlib.h>

#define ACCESS(N, M, i, j) (*((M) + (i) * (N) + (j)))
#define STR(x) #x

#define DType long long
#define SYMBOL(name) determinant_ ## ll_ ## name
#include "plain-inl.h"
#include "simd-ll-inl.h"
#undef SYMBOL
#undef DType

#define DType float
#define SYMBOL(name) determinant_ ## s_ ## name
#include "plain-inl.h"
#include "simd-inl.h"
#undef SYMBOL
#undef DType

#define DType double
#define SYMBOL(name) determinant_ ## d_ ## name
#include "plain-inl.h"
#include "simd-inl.h"
#undef SYMBOL
#undef DType

#ifdef USE_OPENBLAS
#define DType float
#define BLAS_CALL LAPACKE_sgetrf
#define SYMBOL(name) determinant_ ## s_ ## name
#include "blas-inl.h"
#undef SYMBOL
#undef BLAS_CALL
#undef DType

#define DType double
#define BLAS_CALL LAPACKE_dgetrf
#define SYMBOL(name) determinant_ ## d_ ## name
#include "blas-inl.h"
#undef SYMBOL
#undef BLAS_CALL
#undef DType
#endif
