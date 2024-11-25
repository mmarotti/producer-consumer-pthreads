#include <omp.h>
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

void multiply_matrices(
  double matrix1[MATRIX_SIZE][MATRIX_SIZE],
  double matrix2[MATRIX_SIZE][MATRIX_SIZE],
  double result[MATRIX_SIZE][MATRIX_SIZE]
) {
  #pragma omp parallel for collapse(2)
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      result[i][j] = 0;
      for (int k = 0; k < MATRIX_SIZE; k++) {
        result[i][j] += matrix1[i][k] * matrix2[k][j];
      }
    }
  }
}

void kill_threads_NCP2() {
  for (int i = 0; i < NCP2; i++) {
    sbuf_s *kill_item = (sbuf_s *) malloc(sizeof(sbuf_s));
    kill_item->kill = 1;

    /* Prepare to write item to buf */
    /* If there are no empty slots, wait */
    sem_wait(shared[1].empty);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[1].mutex);

    shared[1].buf[shared[1].in] = kill_item;
    shared[1].in = (shared[1].in + 1) % BUFF_SIZE;

    /* Increment the number of full slots */
    sem_post(shared[1].full);
    /* Release the buffer */
    sem_post(shared[1].mutex);
  }
}

void *ConsumerProducer1(void *arg) {
  sbuf_s *item;
  int i, index;

  index = *((int *) arg);

  while (1) {
    /* Prepare to read item from buf */
    /* If there are no filled slots, wait */
    sem_wait(shared[0].full);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[0].mutex);

    item = shared[0].buf[shared[0].out];
    shared[0].out = (shared[0].out + 1) % BUFF_SIZE;

    printf("[CP1_%d] Consuming %s...\n", index, item->name); fflush(stdout);

    /* Increment the number of empty slots */
    sem_post(shared[0].empty);
    /* Release the buffer */
    sem_post(shared[0].mutex);

    /* Generating kill messages for NCP2 threads */
    if (item->kill) {
      kill_threads_NCP2();
      printf("[CP1_%d] Received kill message\n", index); fflush(stdout);
      return NULL;
    }

    /* Multiply matrices */
    multiply_matrices(item->a, item->b, item->c);

    /* Prepare to write item to buf */
    /* If there are no empty slots, wait */
    sem_wait(shared[1].empty);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[1].mutex);

    printf("[CP1_%d] Producing %s...\n", index, item->name); fflush(stdout);

    shared[1].buf[shared[1].in] = item;
    shared[1].in = (shared[1].in+1) % BUFF_SIZE;

    /* Increment the number of full slots */
    sem_post(shared[1].full);
    /* Release the buffer */
    sem_post(shared[1].mutex);
  }

  return NULL;
}