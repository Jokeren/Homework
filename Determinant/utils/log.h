#ifndef _DETERMINANT_UTILS_LOG_H_
#define _DETERMINANT_UTILS_LOG_H_

#include <stdio.h>

#define LOG_INFO(prefix, ...)        \
  do {                               \
    fprintf(stdout, "%s-->", prefix) \
    fprintf(stdout, __VA_ARGS__)     \
  }

#endif  // _DETERMINANT_UTILS_LOG_H_
