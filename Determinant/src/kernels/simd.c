#include <math.h>
#include <stdlib.h>
#include <kernels/plain.h>
#include <utils/log.h>

#define ACCESS(N, M, i, j) (*((M) + (i) * (N) + (j)))

float determinant_s_simd_kernel(size_t N, float *M) {
  if (N == 1) {
    return *M;
  } else if (N == 2) {
    return ACCESS(N, M, 0, 0) * ACCESS(N, M, 1, 1) - ACCESS(N, M, 1, 0) * ACCESS(N, M, 0, 1);
  } else if (N == 3) {
    return ACCESS(N, M, 0, 0) * (ACCESS(N, M, 1, 1) * ACCESS(N, M, 2, 2) - ACCESS(N, M, 1, 2) * ACCESS(N, M, 2, 1)) - 
		   ACCESS(N, M, 0, 1) * (ACCESS(N, M, 1, 0) * ACCESS(N, M, 2, 2) - ACCESS(N, M, 1, 2) * ACCESS(N, M, 2, 0)) +
		   ACCESS(N, M, 0, 2) * (ACCESS(N, M, 1, 0) * ACCESS(N, M, 2, 1) - ACCESS(N, M, 1, 1) * ACCESS(N, M, 2, 0));
  } else {
    size_t i, j, k;
    size_t max_loc;
    float max_val;
    float det = 1.0f;
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
        for (j = i; j < N; ++j) {
          float tmp = ACCESS(N, M, max_loc, j);
          ACCESS(N, M, max_loc, j) = ACCESS(N, M, i, j);
          ACCESS(N, M, i, j) = tmp;
        }
      }
      for (j = i + 1; j < N; ++j) {
        float factor = ACCESS(N, M, j, j) / ACCESS(N, M, i, i);
        for (k = i; k < N; ++k) {
          ACCESS(N, M, j, k) = ACCESS(N, M, j, k) - factor * ACCESS(N, M, i, k);
        }
      }
      det *= ACCESS(N, M, i, i);
    }
    return det;
  }
}


double determinant_d_simd_kernel(size_t N, double *M) {
  if (N == 1) {
    return *M;
  } else if (N == 2) {
    return ACCESS(N, M, 0, 0) * ACCESS(N, M, 1, 1) - ACCESS(N, M, 1, 0) * ACCESS(N, M, 0, 1);
  } else if (N == 3) {
    return ACCESS(N, M, 0, 0) * (ACCESS(N, M, 1, 1) * ACCESS(N, M, 2, 2) - ACCESS(N, M, 1, 2) * ACCESS(N, M, 2, 1)) - 
		   ACCESS(N, M, 0, 1) * (ACCESS(N, M, 1, 0) * ACCESS(N, M, 2, 2) - ACCESS(N, M, 1, 2) * ACCESS(N, M, 2, 0)) +
		   ACCESS(N, M, 0, 2) * (ACCESS(N, M, 1, 0) * ACCESS(N, M, 2, 1) - ACCESS(N, M, 1, 1) * ACCESS(N, M, 2, 0));
  } else {
    size_t i, j, k;
    size_t max_loc;
    double max_val;
    double det = 1.0f;
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
        for (j = i; j < N; ++j) {
          double tmp = ACCESS(N, M, max_loc, j);
          ACCESS(N, M, max_loc, j) = ACCESS(N, M, i, j);
          ACCESS(N, M, i, j) = tmp;
        }
      }
      for (j = i + 1; j < N; ++j) {
        double factor = ACCESS(N, M, j, i) / ACCESS(N, M, i, i);
        for (k = i; k < N; ++k) {
          ACCESS(N, M, j, k) = ACCESS(N, M, j, k) - factor * ACCESS(N, M, i, k);
        }
      }
      det *= ACCESS(N, M, i, i);
    }
    return det;
  }
}

