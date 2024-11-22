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

void sum_vector(
  double vector[MATRIX_SIZE],
  double *e
) {
  *e = 0;
  for (int i = 0; i < MATRIX_SIZE; i++) {
    *e += vector[i];
  }
}

void kill_threads_NC() {
  for (int i = 0; i < NC; i++) {
    sbuf_s *kill_item = (sbuf_s *) malloc(sizeof(sbuf_s));
    kill_item->kill = 1;

    /* Prepare to write item to buf */
    /* If there are no empty slots, wait */
    sem_wait(shared[3].empty);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[3].mutex);

    shared[3].buf[shared[3].in] = kill_item;
    shared[3].in = (shared[3].in + 1) % BUFF_SIZE;

    /* Increment the number of full slots */
    sem_post(shared[3].full);
    /* Release the buffer */
    sem_post(shared[3].mutex);
  }
}

void *ConsumerProducer3(void *arg) {
  sbuf_s *item;
  int i, index;

  index = *((int *) arg);

  while (1) {
    /* Prepare to read item from buf */
    /* If there are no filled slots, wait */
    sem_wait(shared[2].full);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[2].mutex);

    item = shared[2].buf[shared[2].out];
    shared[2].out = (shared[2].out + 1) % BUFF_SIZE;

    printf("[CP3_%d] Consuming %s...\n", index, item->name); fflush(stdout);

    /* Increment the number of empty slots */
    sem_post(shared[2].empty);
    /* Release the buffer */
    sem_post(shared[2].mutex);

    /* Generating kill messages for NCP3 threads */
    if (item->kill) {
      printf("[CP3_%d] Received kill message\n", index); fflush(stdout);
      kill_threads_NC();
      return NULL;;
    }

    /* Sum all elements of vector v into e */
    sum_vector(item->v, &item->e);

    /* Prepare to write item to buf */
    /* If there are no empty slots, wait */
    sem_wait(shared[3].empty);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[3].mutex);

    shared[3].buf[shared[3].in] = item;
    shared[3].in = (shared[3].in + 1) % BUFF_SIZE;

    printf("[CP3_%d] Producing %s (%f)...\n", index, item->name, item->e); fflush(stdout);

    /* Increment the number of full slots */
    sem_post(shared[3].full);
    /* Release the buffer */
    sem_post(shared[3].mutex);
  }

  return NULL;
}