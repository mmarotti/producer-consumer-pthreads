#include <stdio.h>
#include <stdlib.h>
#include "../definitions.h"

/* To avoid multiple includes */
#ifndef BUFFER_INCLUDED
  #define BUFFER_INCLUDED
  #include "../types/buffer_item.c" 
  #include "../types/buffer_stack.c" 
#endif

extern sbuf_t shared[BUFF_NUMBER];

void *ConsumerProducer(void *arg) {
  sbuf_s item;
  int i, index;

  index = *((int *) arg);

  for (i=0; i < NITERS; i++) { /* Prepare to read item from buf */
    /* If there are no filled slots, wait */
    sem_wait(&shared[0].full);
    /* If another thread uses the buffer, wait */
    sem_wait(&shared[0].mutex);

    item = shared[0].buf[shared[0].out];
    shared[0].out = (shared[0].out + 1) % BUFF_SIZE;

    printf("[CP1_%d] Consuming %f ...\n", index, item.e); fflush(stdout);

    /* Increment the number of empty slots */
    sem_post(&shared[0].empty);
    /* Release the buffer */
    sem_post(&shared[0].mutex);

    /* Change consumed item */
    item.e += 500.00;

    /* Prepare to write item to buf */
    /* If there are no empty slots, wait */
    sem_wait(&shared[1].empty);
    /* If another thread uses the buffer, wait */
    sem_wait(&shared[1].mutex);

    shared[1].buf[shared[1].in] = item;
    shared[1].in = (shared[1].in+1) % BUFF_SIZE;

    printf("[CP1_%d] Producing %f ...\n", index, item.e); fflush(stdout);

    /* Increment the number of full slots */
    sem_post(&shared[1].full);
    /* Release the buffer */
    sem_post(&shared[1].mutex);
  }

  return NULL;
}