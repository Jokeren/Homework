// Local include
#include <determinant.h>
// System include
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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


static void get_matrix_ll(const char *file_name, size_t *n, long long **m) {
  FILE *fp;
  size_t size;

  fp = fopen(file_name, "r");
  fscanf(fp, "%zu", n);
  size = *n;
  *m = (long long *)malloc(sizeof(long long) * size * size);

  size_t i, j;
  for (i = 0; i < size; ++i) {
    for (j = 0; j < size; ++j) {
      fscanf(fp, "%lld", *m + i * size + j);
    }
  }

  fclose(fp);
}


int main(int argc, char *argv[]) {
  if (argc != 4) {
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

  if (strcmp(data_type, "float") == 0) {
    float *m;
    determinant_s_fn_t determinant_s_fn = lookup_determinant_func(data_type, kernel_name);
    if (determinant_s_fn != NULL) {
      get_matrix_s(file_name, &n, &m);
      float result = determinant_s_fn(n, m);
      printf("%f\n", result);
    }
    free(m);
  } else if (strcmp(data_type, "double") == 0) {
    double *m;
    determinant_d_fn_t determinant_d_fn = lookup_determinant_func(data_type, kernel_name);
    if (determinant_d_fn != NULL) {
      get_matrix_d(file_name, &n, &m);
      double result = determinant_d_fn(n, m);
      printf("%lf\n", result);
    }
    free(m);
  } else if (strcmp(data_type, "ll") == 0) {
    long long *m;
    determinant_ll_fn_t determinant_ll_fn = lookup_determinant_func("long long", kernel_name);
    if (determinant_ll_fn != NULL) {
      get_matrix_ll(file_name, &n, &m);
      long long result = determinant_ll_fn(n, m);
      printf("%lld\n", result);
    }
    free(m);
  }

  return 0;
}
