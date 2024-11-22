#include "../definitions.h"

typedef struct sbuf_s {
  char name[20];
  double a[MATRIX_SIZE][MATRIX_SIZE];
  double b[MATRIX_SIZE][MATRIX_SIZE];
  double c[MATRIX_SIZE][MATRIX_SIZE];
  double v[MATRIX_SIZE];
  double e;
} sbuf_s;
