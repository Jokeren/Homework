// System include
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// Local include
#include <determinant.h>


void init(size_t n, float *m) {
  srand((unsigned int)time(NULL));
  size_t i, j;
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      *(m + i * n + j) = ((float)rand()/(float)(RAND_MAX));
    }
  }
}


void display(size_t n, float *m) {
  size_t i, j;
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      printf("%f ", *(m + i * n + j));
    }
    printf("\n");
  }
}


int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Arguments error!\n");
    exit(-1);
  }
  size_t n = atoi(argv[1]);
  char *kernel_name = argv[2];
  determinant_s_fn_t determinant_s_fn = lookup_determinant_s(kernel_name);

  float *m;
  init(n, m);
  printf("Init matrix:\n");
  display(n, m);
  float result = determinant_s_fn(n, m);
  printf("Determinant result: %f\n", result);
  return 0;
}
