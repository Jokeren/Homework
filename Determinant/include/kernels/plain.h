#ifndef _INCLUDE_KERNELS_PLAIN_H_
#define _INCLUDE_KERNELS_PLAIN_H_

float determinant_s_plain_kernel(size_t N, float *M);

double determinant_d_plain_kernel(size_t N, double *M);

long long determinant_ll_plain_kernel(size_t N, long long *M);

long long determinant_ll_plain1_kernel(size_t N, long long *M);

long long determinant_ll_plain2_kernel(size_t N, long long *M, size_t offset);

#endif  // _INCLUDE_KERNELS_PLAIN_H_
