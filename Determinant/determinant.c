// Local include
#include <determinant.h>
// System include
#include <stdio.h>
#include <stdlib.h>

static void get_matrix(const char *file_name, size_t *n, float **m) {
  FILE *fp;
  size_t size;

  fp = fopen(file_name, "r");
  fscanf(fp, "%zu", n);
  size = *n;
  *m = (float *)malloc(sizeof(float) * size * size);

  size_t i, j;
  for (i = 0; i < size; ++i) {
    for (j = 0; j < size; ++j) {
      fscanf(fp, "%f", *m + i * size + j);
    }
  }

  fclose(fp);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Arguments error!\n");
    fprintf(stderr, "arg[1]: file_name!\n");
    fprintf(stderr, "arg[2]: kernel_name!\n");
    exit(1);
  }

  size_t n;
  float *m;
  char *file_name = argv[1];
  char *kernel_name = argv[2];
  determinant_s_fn_t determinant_s_fn = lookup_determinant_s(kernel_name);
  get_matrix(file_name, &n, &m);

  float result = determinant_s_fn(n, m);
  printf("%f\n", result);
  free(m);
  return 0;
}
