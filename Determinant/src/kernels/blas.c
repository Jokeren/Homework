#include <stdlib.h>
#include <kernels/plain.h>
#include <utils/log.h>
#if BLAS == openblas
#include <cblas.h>
#elif BLAS == mkl
#include <mkl.h>
#endif


float determinant_s_blas_kernel(size_t N, const float *M) {
}


double determinant_d_blas_kernel(size_t N, const double *M) {
}

