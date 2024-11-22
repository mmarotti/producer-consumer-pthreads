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

void calculate_vector(
  double matrix[MATRIX_SIZE][MATRIX_SIZE],
  double vector[MATRIX_SIZE]
) {
  for (int j = 0; j < MATRIX_SIZE; j++) {
    vector[j] = 0;
    for (int i = 0; i < MATRIX_SIZE; i++) {
      vector[j] += matrix[i][j];
    }
  }
}

void *ConsumerProducer2(void *arg) {
  sbuf_s item;
  int i, index;

  index = *((int *) arg);

  for (i=0; i < NITERS; i++) { /* Prepare to read item from buf */
    /* If there are no filled slots, wait */
    sem_wait(shared[1].full);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[1].mutex);

    item = shared[1].buf[shared[1].out];
    shared[1].out = (shared[1].out + 1) % BUFF_SIZE;

    printf("[CP2_%d] Consuming %s...\n", index, item.name); fflush(stdout);

    /* Increment the number of empty slots */
    sem_post(shared[1].empty);
    /* Release the buffer */
    sem_post(shared[1].mutex);

    /* Calculate vector */
    calculate_vector(item.a, item.v);

    /* Prepare to write item to buf */
    /* If there are no empty slots, wait */
    sem_wait(shared[2].empty);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[2].mutex);

    printf("[CP2_%d] Producing %s...\n", index, item.name); fflush(stdout);

    shared[2].buf[shared[2].in] = item;
    shared[2].in = (shared[2].in+1) % BUFF_SIZE;

    /* Increment the number of full slots */
    sem_post(shared[2].full);
    /* Release the buffer */
    sem_post(shared[2].mutex);
  }

  return NULL;
}