#include <math.h>
#include <stdlib.h>
#include <kernels/plain.h>
#include <utils/log.h>

#define ACCESS(N, M, i, j) (*((M) + (i) * (N) + (j)))

float determinant_s_simd_kernel(size_t N, float *M) {
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
            det *= -1.0f;
            for (j = i; j < N; ++j) {
                float tmp = ACCESS(N, M, max_loc, j);
                ACCESS(N, M, max_loc, j) = ACCESS(N, M, i, j);
                ACCESS(N, M, i, j) = tmp;
            }
        }
        for (j = i + 1; j < N; ++j) {
            float fraction = ACCESS(N, M, j, j) / ACCESS(N, M, i, i);
            for (k = i; k < N; ++k) {
                ACCESS(N, M, j, k) = ACCESS(N, M, j, k) - fraction * ACCESS(N, M, i, k);
            }
        }
        det *= ACCESS(N, M, i, i);
    }
    return det;
}


double determinant_d_simd_kernel(size_t N, double *M) {
    size_t i, j, k;
    size_t max_loc;
    double max_val;
    double det = 1.0;
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
            det *= -1.0;
            for (j = i; j < N; ++j) {
                double tmp = ACCESS(N, M, max_loc, j);
                ACCESS(N, M, max_loc, j) = ACCESS(N, M, i, j);
                ACCESS(N, M, i, j) = tmp;
            }
        }
        for (j = i + 1; j < N; ++j) {
            double fraction = ACCESS(N, M, j, i) / ACCESS(N, M, i, i);
            for (k = i; k < N; ++k) {
                ACCESS(N, M, j, k) = ACCESS(N, M, j, k) - fraction * ACCESS(N, M, i, k);
            }
        }
        det *= ACCESS(N, M, i, i);
    }
    return det;
}

