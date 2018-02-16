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
    m = (float *)malloc(sizeof(float) * (N - 1) * (N - 1));
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
        det += factor * ACCESS(N, M, 0, pivot) * determinant_s_plain_kernel(N - 1, m);
      }
      factor = -factor;
    }
    free(m);
    return det;
  }
}


double determinant_d_plain_kernel(size_t N, double *M) {
  if (N == 1) {
    return *M;
  } else if (N == 2) {
    return ACCESS(N, M, 0, 0) * ACCESS(N, M, 1, 1) - ACCESS(N, M, 1, 0) * ACCESS(N, M, 0, 1);
  } else {
    size_t pivot;
    double *m;
    double factor = 1.0f;
    double det = 0.0f;
    m = (double *)malloc(sizeof(double) * (N - 1) * (N - 1));
    for (pivot = 0; pivot < N; ++pivot) {
      if (ACCESS(N, M, 0, pivot) != 0.0) {
        size_t i, j;
        for (i = 1; i < N; ++i) {
          for (j = 0; j < pivot; ++j) {
            ACCESS(N - 1, m, i - 1, j) = ACCESS(N, M, i, j);
          }
          for (j = pivot + 1; j < N; ++j) {
            ACCESS(N - 1, m, i - 1, j - 1) = ACCESS(N, M, i, j);
          }
        } 
        det += factor * ACCESS(N, M, 0, pivot) * determinant_d_plain_kernel(N - 1, m);
      }
      factor = -factor;
    }
    free(m);
    return det;
  }
}


long long determinant_ll_plain_kernel(size_t N, long long *M) {
  if (N == 1) {
    return *M;
  } else if (N == 2) {
    return ACCESS(N, M, 0, 0) * ACCESS(N, M, 1, 1) - ACCESS(N, M, 1, 0) * ACCESS(N, M, 0, 1);
  } else {
    size_t pivot;
    long long *m;
    long long factor = 1;
    long long det = 0;
    m = (long long *)malloc(sizeof(long long) * (N - 1) * (N - 1));
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
        det += factor * ACCESS(N, M, 0, pivot) * determinant_ll_plain_kernel(N - 1, m);
      }
      factor = -factor;
    }
    free(m);
    return det;
  }
}


long long determinant_ll_plain1_kernel(size_t N, long long *M) {
  if (N == 1) {
    return *M;
  } else if (N == 2) {
    return ACCESS(N, M, 0, 0) * ACCESS(N, M, 1, 1) - ACCESS(N, M, 1, 0) * ACCESS(N, M, 0, 1);
  } else if (N == 3) {
    return ACCESS(N, M, 0, 0) * (ACCESS(N, M, 1, 1) * ACCESS(N, M, 2, 2) - ACCESS(N, M, 1, 2) * ACCESS(N, M, 2, 1)) - 
		   ACCESS(N, M, 0, 1) * (ACCESS(N, M, 1, 0) * ACCESS(N, M, 2, 2) - ACCESS(N, M, 1, 2) * ACCESS(N, M, 2, 0)) +
		   ACCESS(N, M, 0, 2) * (ACCESS(N, M, 1, 0) * ACCESS(N, M, 2, 1) - ACCESS(N, M, 1, 1) * ACCESS(N, M, 2, 0));
  } else {
    size_t pivot;
    long long *m;
    long long factor = 1;
    long long det = 0;
    m = (long long *)malloc(sizeof(long long) * (N - 1) * (N - 1));
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
        det += factor * ACCESS(N, M, 0, pivot) * determinant_ll_plain1_kernel(N - 1, m);
      }
      factor = -factor;
    }
    free(m);
    return det;
  }
}


long long determinant_ll_plain2_kernel(size_t N, long long *M, size_t offset) {
  if (N == 1) {
    return *M;
  } else if (N == 2) {
    return ACCESS(N, M, 0, 0) * ACCESS(N, M, 1, 1) - ACCESS(N, M, 1, 0) * ACCESS(N, M, 0, 1);
  } else if (N == 3) {
    return ACCESS(N, M, 0, 0) * (ACCESS(N, M, 1, 1) * ACCESS(N, M, 2, 2) - ACCESS(N, M, 1, 2) * ACCESS(N, M, 2, 1)) - 
		   ACCESS(N, M, 0, 1) * (ACCESS(N, M, 1, 0) * ACCESS(N, M, 2, 2) - ACCESS(N, M, 1, 2) * ACCESS(N, M, 2, 0)) +
		   ACCESS(N, M, 0, 2) * (ACCESS(N, M, 1, 0) * ACCESS(N, M, 2, 1) - ACCESS(N, M, 1, 1) * ACCESS(N, M, 2, 0));
  } else {
    size_t pivot;
    long long factor = 1;
    long long det = 0;
    long long *m = M + offset;
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
        det += factor * ACCESS(N, M, 0, pivot) * determinant_ll_plain2_kernel(N - 1, m, offset + (N - 1) * (N - 1));
      }
      factor = -factor;
    }
    return det;
  }
}


long long determinant_ll_plain3_kernel(size_t N, long long *M, size_t offset) {
  if (N == 1) {
    return *M;
  } else if (N == 2) {
    return ACCESS(N, M, 0, 0) * ACCESS(N, M, 1, 1) - ACCESS(N, M, 1, 0) * ACCESS(N, M, 0, 1);
  } else if (N == 3) {
    return ACCESS(N, M, 0, 0) * (ACCESS(N, M, 1, 1) * ACCESS(N, M, 2, 2) - ACCESS(N, M, 1, 2) * ACCESS(N, M, 2, 1)) - 
		   ACCESS(N, M, 0, 1) * (ACCESS(N, M, 1, 0) * ACCESS(N, M, 2, 2) - ACCESS(N, M, 1, 2) * ACCESS(N, M, 2, 0)) +
		   ACCESS(N, M, 0, 2) * (ACCESS(N, M, 1, 0) * ACCESS(N, M, 2, 1) - ACCESS(N, M, 1, 1) * ACCESS(N, M, 2, 0));
  } else {
    size_t pivot;
    long long factor = 1;
    long long det = 0;
    long long *m = M + offset;
    size_t i, j;
    pivot = 0;
    for (i = 1; i < N; ++i) {
      for (j = 0; j < pivot; ++j) {
        ACCESS(N - 1, m, i - 1, j) = ACCESS(N, M, i, j);
      }
      for (j = pivot + 1; j < N; ++j) {
        ACCESS(N - 1, m, i - 1, j - 1) = ACCESS(N, M, i, j);
      }
    } 
    if (ACCESS(N, M, 0, pivot) != 0)
      det += factor * ACCESS(N, M, 0, pivot) * determinant_ll_plain3_kernel(N - 1, m, offset + (N - 1) * (N - 1));
    factor = -factor;
    for (pivot = 1; pivot < N; ++pivot) {
      for (i = 1; i < N; ++i) {
        ACCESS(N - 1, m, i - 1, pivot - 1) = ACCESS(N, M, i, pivot - 1);
      }
      if (ACCESS(N, M, 0, pivot) != 0)
        det += factor * ACCESS(N, M, 0, pivot) * determinant_ll_plain3_kernel(N - 1, m, offset + (N - 1) * (N - 1));
      factor = -factor;
    }
    return det;
  }
}
