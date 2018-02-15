#include <stdlib.h>
#include <kernels/plain.h>
#include <utils/log.h>
#ifdef USE_OPENBLAS
#include <lapacke.h>
#endif

#define ACCESS(N, M, i, j) (*((M) + (i) * (N) + (j)))

#define LAPACKE_CALL(prefix, func)               \
  do {                                           \
    int res = 0;                                 \
    if ((res = func) < 0) {                      \
      LOG_ERROR(prefix, "return code %d", res);  \
    }                                            \
  } while (0)

float determinant_s_blas_kernel(size_t N, float *M) {
}


double determinant_d_blas_kernel(size_t N, double *M) {
#ifdef USE_OPENBLAS
  int *ipiv = (int *)malloc((N + 1) * sizeof(int));
  LAPACKE_CALL("determinant_d_blas_kernel",
               LAPACKE_dgetrf(LAPACK_ROW_MAJOR, N, N, M, N, ipiv));
  int sum = 0;
  double det = 1.0;
  size_t i;
  for (i = 0; i < N; ++i) {
    sum += (ipiv[i] - 1) ^ i;
    det *= ACCESS(N, M, i, i);
  }
  det = sum % 2 ? -det : det;
  return det;
#else
  return 0.0;
#endif
}
