#ifndef _INCLUDE_KERNELS_SIMD_H_
#define _INCLUDE_KERNELS_SIMD_H_

long long determinant_ll_simd_kernel(size_t N, long long *M);

float determinant_s_simd_kernel(size_t N, float *M);

double determinant_d_simd_kernel(size_t N, double *M);

#endif  // _INCLUDE_KERNELS_SIMD_H_
