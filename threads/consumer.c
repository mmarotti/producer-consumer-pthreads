#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "../definitions.h"

/* To avoid multiple includes */
#ifndef BUFFER_INCLUDED
  #define BUFFER_INCLUDED
  #include "../types/buffer_item.c" 
  #include "../types/buffer_stack.c" 
#endif

extern sbuf_t shared[BUFF_NUMBER];

void write_output(sbuf_s* item, int first_write) {
  FILE *output_file;
  
  if (first_write) {
    output_file = fopen(OUTPUT_FILENAME, "w");
  } else {
    output_file = fopen(OUTPUT_FILENAME, "a");
  }

  if (output_file == NULL) {
    perror("Error opening output file");
    return;
  }

  fprintf(output_file, "=========================================================================================\n");
  fprintf(output_file, "Entrada: %s;\n", item->name);
  fprintf(output_file, "——————————–——————————–——————————–——————————–——————————–——————————–——————————–——————————––\n");
  fprintf(output_file, "A\n");
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      fprintf(output_file, "%.6f ", item->a[i][j]);
    }
    fprintf(output_file, "\n");
  }
  fprintf(output_file, "——————————–——————————–——————————–——————————–——————————–——————————–——————————–——————————––\n");
  fprintf(output_file, "B\n");
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      fprintf(output_file, "%.6f ", item->b[i][j]);
    }
    fprintf(output_file, "\n");
  }
  fprintf(output_file, "——————————–——————————–——————————–——————————–——————————–——————————–——————————–——————————––\n");
  fprintf(output_file, "C\n");
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      fprintf(output_file, "%.6f ", item->c[i][j]);
    }
    fprintf(output_file, "\n");
  }
  fprintf(output_file, "——————————–——————————–——————————–——————————–——————————–——————————–——————————–——————————––\n");
  fprintf(output_file, "V\n");
  for (int i = 0; i < MATRIX_SIZE; i++) {
    fprintf(output_file, "%.6f ", item->v[i]);
  }
  fprintf(output_file, "\n");
  fprintf(output_file, "——————————–——————————–——————————–——————————–——————————–——————————–——————————–——————————––\n");
  fprintf(output_file, "E\n");
  fprintf(output_file, "%.6f\n", item->e);
  fprintf(output_file, "=========================================================================================\n\n");

  fclose(output_file);
}

void *Consumer(void *arg) {
  sbuf_s *item;
  int i, index;
  static int first_write = 1;

  index = *((int *) arg);

  while (1) {
    /* Prepare to read item from buf */
    /* If there are no filled slots, wait */
    sem_wait(shared[3].full);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[3].mutex); 

    item = shared[3].buf[shared[3].out];
    shared[3].out = (shared[3].out + 1) % BUFF_SIZE;

    printf("[C_%d] Consuming %s...\n", index, item->name); fflush(stdout);

    if (item->kill) {
      printf("[C_%d] Received kill message\n", index); fflush(stdout);
      return NULL;
    }

    /* Write output to file */
    write_output(item, first_write);

    first_write = 0;
    /* Release the buffer */
    sem_post(shared[3].mutex);
    /* Increment the number of empty slots */
    sem_post(shared[3].empty);
  }

  return NULL;
}
