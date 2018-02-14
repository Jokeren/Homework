#ifndef _INCLUDE_KERNELS_BLAS_H_
#define _INCLUDE_KERNELS_BLAS_H_

float determinant_s_blas_kernel(size_t N, const float *M);

double determinant_d_blas_kernel(size_t N, const double *M);

#endif  // _INCLUDE_KERNELS_BLAS_H_

