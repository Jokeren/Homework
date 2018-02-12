#ifndef _INCLUDE_UTILS_LOG_H_
#define _INCLUDE_UTILS_LOG_H_

#include <stdio.h>
#include <stdlib.h>

#define LOG_INFO(prefix, ...)         \
  do {                                \
    fprintf(stdout, "%s-->", prefix); \
    fprintf(stdout, __VA_ARGS__);     \
    fprintf(stdout, "\n");            \
  } while(0)

#define LOG_ERROR(prefix, ...)        \
  do {                                \
    fprintf(stderr, "%s-->", prefix); \
    fprintf(stderr, __VA_ARGS__);     \
    fprintf(stderr, "\n");            \
    exit(1);                         \
  } while(0)

#endif  // _INCLUDE_UTILS_LOG_H_
