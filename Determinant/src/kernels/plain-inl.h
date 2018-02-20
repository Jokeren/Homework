#include <stdlib.h>
#include <kernels/plain.h>
#include <utils/log.h>

DType SYMBOL(plain_kernel)(size_t N, DType *M) {
  if (N == 1) {
    return *M;
  } else if (N == 2) {
    return ACCESS(N, M, 0, 0) * ACCESS(N, M, 1, 1) - ACCESS(N, M, 1, 0) * ACCESS(N, M, 0, 1);
  } else {
    size_t pivot;
    DType *m;
    DType factor = 1;
    DType det = 0;
    m = (DType *)malloc(sizeof(DType) * (N - 1) * (N - 1));
    for (pivot = 0; pivot < N; ++pivot) {
      if (ACCESS(N, M, 0, pivot) != 0) {
        size_t i, j;
        for (i = 1; i < N; ++i) {
          for (j = 0; j < pivot; ++j) {
            ACCESS(N - 1, m, i - 1, j) = ACCESS(N, M, i, j);
          }
          for (j = pivot + 1; j < N; ++j) {
            ACCESS(N - 1, m, i - 1, j - 1) = ACCESS(N, M, i, j);
          }
        } 
        det += factor * ACCESS(N, M, 0, pivot) * SYMBOL(plain_kernel)(N - 1, m);
      }
      factor = -factor;
    }
    free(m);
    return det;
  }
}
