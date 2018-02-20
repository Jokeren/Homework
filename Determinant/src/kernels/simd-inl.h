#include <math.h>
#include <stdlib.h>
#include <kernels/simd.h>
#include <utils/log.h>

DType SYMBOL(simd_kernel)(size_t N, DType *M) {
  size_t i, j, k;
  size_t max_loc;
  DType max_val;
  DType det = 1;
  for (i = 0; i < N; ++i) {
    max_loc = i;
    max_val = fabs(ACCESS(N, M, i, i));
    for (j = i + 1; j < N; ++j) {
      if (fabs(ACCESS(N, M, j, i)) > max_val) {
        max_val = fabs(ACCESS(N, M, j, i));
        max_loc = j;
      }
    }
    if (max_loc != i) {
      det *= -1;
      for (j = i; j < N; ++j) {
        DType tmp = ACCESS(N, M, max_loc, j);
        ACCESS(N, M, max_loc, j) = ACCESS(N, M, i, j);
        ACCESS(N, M, i, j) = tmp;
      }
    }
    for (j = i + 1; j < N; ++j) {
      DType fraction = ACCESS(N, M, j, i) / ACCESS(N, M, i, i);
      for (k = i; k < N; ++k) {
        ACCESS(N, M, j, k) = ACCESS(N, M, j, k) - fraction * ACCESS(N, M, i, k);
      }
    }
    det *= ACCESS(N, M, i, i);
  }
  return det;
}
