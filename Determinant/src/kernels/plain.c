#include <stdlib.h>
#include <kernels/plain.h>
#include <utils/log.h>

#define ACCESS(N, M, i, j) (*((M) + (i) * (N) + (j)))

float determinant_s_plain_kernel(size_t N, float *M) {
  if (N == 1) {
    return *M;
  } else if (N == 2) {
    return ACCESS(N, M, 0, 0) * ACCESS(N, M, 1, 1) - ACCESS(N, M, 1, 0) * ACCESS(N, M, 0, 1);
  } else {
    size_t pivot;
    float *m;
    float factor = 1.0f;
    float det = 0.0f;
    for (pivot = 0; pivot < N; ++pivot) {
      m = (float *)malloc(sizeof(float) * (N - 1) * (N - 1));
      size_t i, j;
      for (i = 1; i < N; ++i) {
        for (j = 0; j < pivot; ++j) {
          ACCESS(N - 1, m, i - 1, j) = ACCESS(N, M, i, j);
        }
        for (j = pivot + 1; j < N; ++j) {
          ACCESS(N - 1, m, i - 1, j - 1) = ACCESS(N, M, i, j);
        }
      } 
      det += factor * ACCESS(N, M, 0, pivot) * determinant_s_plain_kernel(N - 1, m);
      factor = -factor;
      free(m);
    }
    return det;
  }
}
