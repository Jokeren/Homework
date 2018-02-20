#include <stdlib.h>
#include <kernels/blas.h>
#include <utils/log.h>
#include <lapacke.h>

#ifndef LAPACKE_CALL
#define LAPACKE_CALL(prefix, func)               \
  do {                                           \
    int res = 0;                                 \
    if ((res = func) < 0) {                      \
      LOG_ERROR(prefix, "return code %d", res);  \
    }                                            \
  } while (0)
#endif

DType SYMBOL(blas_kernel)(size_t N, DType *M) {
  int *ipiv = (int *)malloc((N + 1) * sizeof(int));
  LAPACKE_CALL(STR(SYMBOL(blas_kernel)),
               BLAS_CALL(LAPACK_ROW_MAJOR, N, N, M, N, ipiv));
  int sum = 0;
  DType det = 1.0;
  size_t i;
  for (i = 0; i < N; ++i) {
    sum += (ipiv[i] - 1) ^ i;
    det *= ACCESS(N, M, i, i);
  }
  det = sum % 2 ? -det : det;
  return det;
}
