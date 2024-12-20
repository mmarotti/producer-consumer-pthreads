#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* For strcpy and memcpy */
#include "../definitions.h"

/* To avoid multiple includes */
#ifndef BUFFER_INCLUDED
  #define BUFFER_INCLUDED
  #include "../types/buffer_item.c" 
  #include "../types/buffer_stack.c" 
#endif

extern sbuf_t shared[BUFF_NUMBER];

void read_matrix(
  FILE *file,
  double matrix[MATRIX_SIZE][MATRIX_SIZE]
) {
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      fscanf(file, "%lf", &matrix[i][j]);
    }
  }
}

void kill_threads_NCP1() {
  for (int i = 0; i < NCP1; i++) {
    sbuf_s *kill_item = (sbuf_s *) malloc(sizeof(sbuf_s));
    kill_item->kill = 1;

    /* Prepare to write item to buf */
    /* If there are no empty slots, wait */
    sem_wait(shared[0].empty);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[0].mutex);

    shared[0].buf[shared[0].in] = kill_item;
    shared[0].in = (shared[0].in + 1) % BUFF_SIZE;

    /* Increment the number of full slots */
    sem_post(shared[0].full);
    /* Release the buffer */
    sem_post(shared[0].mutex);
  }
}

void *Producer(void *arg) {
  sbuf_s *item;
  int i, index;

  index = *((int *)arg);

  FILE *list_file = fopen(INPUT_FILENAME, "r");

  if (list_file == NULL) {
    perror("Error opening input list file");
    return NULL;
  }
  
  char filename[MAX_FILENAME_LENGTH];

  while (fgets(filename, MAX_FILENAME_LENGTH, list_file) != NULL) {
    /* Prepare to write item to buf */
    /* If there are no empty slots, wait */
    sem_wait(shared[0].empty);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[0].mutex);

    double matrix1[MATRIX_SIZE][MATRIX_SIZE];
    double matrix2[MATRIX_SIZE][MATRIX_SIZE];

    /* Remove newline character from filename */
    filename[strcspn(filename, "\n")] = '\0';

    FILE *input_file = fopen(filename, "r");

    if (input_file == NULL) {
      perror("Error opening input file");
      return NULL;
    }

    read_matrix(input_file, matrix1);
    fgetc(input_file); /* Read the newline character between matrices */
    read_matrix(input_file, matrix2);

    fclose(input_file);

    item = (sbuf_s *) malloc(sizeof(sbuf_s));
    
    strcpy(item->name, filename);
    memcpy(item->a, matrix1, sizeof(matrix1));
    memcpy(item->b, matrix2, sizeof(matrix2));
    memset(item->c, 0, sizeof(item->c));
    memset(item->v, 0, sizeof(item->v));
    item->e = 0;
    item->kill = 0;

    printf("[P_%d] Producing %s...\n", index, item->name); fflush(stdout);

    shared[0].buf[shared[0].in] = item;
    shared[0].in = (shared[0].in + 1) % BUFF_SIZE;

    /* Release the buffer */
    sem_post(shared[0].mutex);
    /* Increment the number of full slots */
    sem_post(shared[0].full);
  }

  /* Generating kill messages for NCP1 threads */
  kill_threads_NCP1();

  fclose(list_file);

  return NULL;
}