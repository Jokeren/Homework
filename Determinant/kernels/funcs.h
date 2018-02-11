#ifndef _DETERMINANT_KERNELS_FUNCS_H_
#define _DETERMINANT_KERNELS_FUNCS_H_

typedef float (*determinant_s_fn_t)(float *M);

extern *determinant_s_fn_t lookup_determinant_s(char *kernel_name);

#endif  // _DETERMINANT_KERNELS_KERNEL_H_
