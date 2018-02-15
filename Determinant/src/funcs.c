// Header include
#include <determinant.h>
// System include 
#include <string.h>
// Local include
#include <kernels/plain.h>
#include <kernels/blas.h>
#include <utils/timer.h>
#include <utils/log.h>

// Forward declaration
static float determinant_s_plain(size_t N, float *M);

static double determinant_d_plain(size_t N, double *M);

static long long determinant_ll_plain(size_t N, long long *M);

static float determinant_s_blas(size_t N, float *M);

static double determinant_d_blas(size_t N, double *M);

#define FOREACH_FUNC_NAME(macro)      \
  macro("plain", determinant_s_plain) \
  macro("blas", determinant_s_blas)

determinant_s_fn_t lookup_determinant_s(const char *kernel_name) {
#define find_determinant_func(name, func) \
  if (strcmp(kernel_name, name) == 0) {   \
    return func;                          \
  }
  FOREACH_FUNC_NAME(find_determinant_func)
#undef find_determinant_func
}

#undef FOREACH_FUNC_NAME


#define FOREACH_FUNC_NAME(macro)      \
  macro("plain", determinant_d_plain) \
  macro("blas", determinant_d_blas)

determinant_d_fn_t lookup_determinant_d(const char *kernel_name) {
#define find_determinant_func(name, func) \
  if (strcmp(kernel_name, name) == 0) {   \
    return func;                          \
  }
  FOREACH_FUNC_NAME(find_determinant_func)
#undef find_determinant_func
}
#undef FOREACH_FUNC_NAME


#define FOREACH_FUNC_NAME(macro)      \
  macro("plain", determinant_ll_plain)

determinant_ll_fn_t lookup_determinant_ll(const char *kernel_name) {
#define find_determinant_func(name, func) \
  if (strcmp(kernel_name, name) == 0) {   \
    return func;                          \
  }
  FOREACH_FUNC_NAME(find_determinant_func)
#undef find_determinant_func
}
#undef FOREACH_FUNC_NAME



float determinant_s_plain(size_t N, float *M) {
#ifdef PERFORMANCE
  float start = 0.0f;
  float end = 0.0f;
  float elapsed = 0.0f;
  CPU_TIMER_START(elapsed, start);
#endif
  if (N == 0) {
    LOG_ERROR("determinant_s_plain", "N: %zu", N);
  } else {
    LOG_INFO("determinant_s_plain", "N: %zu", N);
  }
  float result = determinant_s_plain_kernel(N, M);
#ifdef PERFORMANCE
  CPU_TIMER_END(elapsed, start, end);
  LOG_INFO("determinant_s_plain", "elapsed time: %f", elapsed);
#endif
  return result;
}


double determinant_d_plain(size_t N, double *M) {
#ifdef PERFORMANCE
  float start = 0.0f;
  float end = 0.0f;
  float elapsed = 0.0f;
  CPU_TIMER_START(elapsed, start);
#endif
  if (N == 0) {
    LOG_ERROR("determinant_d_plain", "N: %zu", N);
  } else {
    LOG_INFO("determinant_d_plain", "N: %zu", N);
  }
  double result = determinant_d_plain_kernel(N, M);
#ifdef PERFORMANCE
  CPU_TIMER_END(elapsed, start, end);
  LOG_INFO("determinant_d_plain", "elapsed time: %f", elapsed);
#endif
  return result;
}


long long determinant_ll_plain(size_t N, long long *M) {
#ifdef PERFORMANCE
  float start = 0.0f;
  float end = 0.0f;
  float elapsed = 0.0f;
  CPU_TIMER_START(elapsed, start);
#endif
  if (N == 0) {
    LOG_ERROR("determinant_ll_plain", "N: %zu", N);
  } else {
    LOG_INFO("determinant_ll_plain", "N: %zu", N);
  }
  long long result = determinant_ll_plain_kernel(N, M);
#ifdef PERFORMANCE
  CPU_TIMER_END(elapsed, start, end);
  LOG_INFO("determinant_d_plain", "elapsed time: %f", elapsed);
#endif
  return result;
}


float determinant_s_blas(size_t N, float *M) {
#ifdef PERFORMANCE
  float start = 0.0f;
  float end = 0.0f;
  float elapsed = 0.0f;
  CPU_TIMER_START(elapsed, start);
#endif
  if (N == 0) {
    LOG_ERROR("determinant_s_blas", "N: %zu", N);
  } else {
    LOG_INFO("determinant_s_blas", "N: %zu", N);
  }
  float result = determinant_s_blas_kernel(N, M);
#ifdef PERFORMANCE
  CPU_TIMER_END(elapsed, start, end);
  LOG_INFO("determinant_s_blas", "elapsed time: %f", elapsed);
#endif
  return result;
}


double determinant_d_blas(size_t N, double *M) {
#ifdef PERFORMANCE
  float start = 0.0f;
  float end = 0.0f;
  float elapsed = 0.0f;
  CPU_TIMER_START(elapsed, start);
#endif
  if (N == 0) {
    LOG_ERROR("determinant_d_blas", "N: %zu", N);
  } else {
    LOG_INFO("determinant_d_blas", "N: %zu", N);
  }
  double result = determinant_d_blas_kernel(N, M);
#ifdef PERFORMANCE
  CPU_TIMER_END(elapsed, start, end);
  LOG_INFO("determinant_d_blas", "elapsed time: %f", elapsed);
#endif
  return result;
}
