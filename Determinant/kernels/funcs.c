// Header include
#include "funcs.h"
// System include 
#include <string.h>
// Local include
#include "plain.h"
#include "../utils/timer.h"
#include "../utils/log.h"

// Forward declaration
static float determinant_s_plain(float *M);

#define FOREACH_FUNC_NAME(macro) \
  macro("plain", determinant_s_plain)

*determinant_s_fn_t lookup_determinant_s(char *kernel_name) {
#define find_determinant_func(name, func) \
  if (strcmp(kernel_name, name) == 0) {   \
    return func;                          \
  }
  FOREACH_FUNC_NAME(find_determinant_func)
#undef find_determinant_func
}

float determinant_s_plain(float *M) {
#ifdef PERFORMANCE
  float start = 0.0f;
  float end = 0.0f;
  float elapsed = 0.0f;
  CPU_TIMER_START(elapsed, start);
#endif
  determinant_s_plain_kernel(M);
#ifdef PERFORMANCE
  CPU_TIMER_END(elapsed, start, end);
  LOG_INFO("Plain kernel", "elapsed time: %f\n", elapsed);
#endif
}

#undef FOREACH_FUNC_NAME
