// Header include
#include <determinant.h>
// System include 
#include <string.h>
// Local include
#include <kernels/plain.h>
#include <utils/timer.h>
#include <utils/log.h>

// Forward declaration
static float determinant_s_plain(size_t N, float *M);

#define FOREACH_FUNC_NAME(macro) \
  macro("plain", determinant_s_plain)

determinant_s_fn_t lookup_determinant_s(char *kernel_name) {
#define find_determinant_func(name, func) \
  if (strcmp(kernel_name, name) == 0) {   \
    return func;                          \
  }
  FOREACH_FUNC_NAME(find_determinant_func)
#undef find_determinant_func
}


float determinant_s_plain(size_t N, float *M) {
#ifdef PERFORMANCE
  float start = 0.0f;
  float end = 0.0f;
  float elapsed = 0.0f;
  CPU_TIMER_START(elapsed, start);
#endif
  if (N == 0) {
    LOG_ERROR("determinant_s_plain", "N: %zu", N);
  }
  float result = determinant_s_plain_kernel(N, M);
#ifdef PERFORMANCE
  CPU_TIMER_END(elapsed, start, end);
  LOG_INFO("determinant_s_plain", "elapsed time: %f", elapsed);
#endif
  return result;
}
