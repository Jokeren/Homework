// Local include
#include <determinant.h>
// System include
#include <stdio.h>
#include <stdlib.h>


static void get_matrix_s(const char *file_name, size_t *n, float **m) {
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


static void get_matrix_d(const char *file_name, size_t *n, double **m) {
  FILE *fp;
  size_t size;

  fp = fopen(file_name, "r");
  fscanf(fp, "%zu", n);
  size = *n;
  *m = (double *)malloc(sizeof(double) * size * size);

  size_t i, j;
  for (i = 0; i < size; ++i) {
    for (j = 0; j < size; ++j) {
      fscanf(fp, "%lf", *m + i * size + j);
    }
  }

  fclose(fp);
}


int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Arguments error!\n");
    fprintf(stderr, "arg[1]: file_name!\n");
    fprintf(stderr, "arg[2]: data_type!\n");
    fprintf(stderr, "arg[3]: kernel_name!\n");
    exit(1);
  }

  size_t n;
  char *file_name = argv[1];
  char *data_type = argv[2];
  char *kernel_name = argv[3];

  if (data_type == "float") {
    float *m;
    determinant_s_fn_t determinant_s_fn = lookup_determinant_s(kernel_name);
    get_matrix_s(file_name, &n, &m);
    float result = determinant_s_fn(n, m);
    printf("%f\n", result);
    free(m);
  } else if (data_type == "double") {
    double *m;
    determinant_d_fn_t determinant_d_fn = lookup_determinant_d(kernel_name);
    get_matrix_d(file_name, &n, &m);
    double result = determinant_d_fn(n, m);
    printf("%lf\n", result);
    free(m);
  }

  return 0;
}
