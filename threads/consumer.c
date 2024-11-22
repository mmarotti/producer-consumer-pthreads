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

void *Consumer(void *arg) {
  sbuf_s item;
  int i, index;

  index = *((int *) arg);

  for (i=0; i < NITERS; i++) { /* Prepare to read item from buf */
    /* If there are no filled slots, wait */
    sem_wait(shared[2].full);
    /* If another thread uses the buffer, wait */
    sem_wait(shared[2].mutex); 

    item = shared[2].buf[shared[2].out];
    shared[2].out = (shared[2].out + 1) % BUFF_SIZE;

    printf("[C_%d] Consuming %s...\n", index, item.name); fflush(stdout);

    /* Release the buffer */
    sem_post(shared[2].mutex);
    /* Increment the number of empty slots */
    sem_post(shared[2].empty);
  }

  return NULL;
}
