#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "definitions.h"

/* To avoid multiple includes */
#ifndef BUFFER_INCLUDED
  #define BUFFER_INCLUDED
  #include "types/buffer_item.c" 
  #include "types/buffer_stack.c" 
#endif

#include "threads/producer.c"
#include "threads/consumer_producer.c"
#include "threads/consumer.c"

int main() {
  pthread_t idP, idC, idCP;
  int index;
  int sP[NP], sC[NC], sCP[NCP];

  for (index=0; index < 2; index++) {
    sem_init(&shared[index].full, 0, 0);
    sem_init(&shared[index].empty, 0, BUFF_SIZE);
    sem_init(&shared[index].mutex, 0, 1);
  }

  for (index = 0; index < NP; index++) {
    sP[index]=index;
    /* Create a new producer */
    pthread_create(&idP, NULL, Producer, &sP[index]);
  }

  for (index = 0; index < NCP; index++) {
    sCP[index]=index;
    /* Create a new consumer producer */
    pthread_create(&idCP, NULL, ConsumerProducer, &sCP[index]);
  }

  for (index = 0; index < NC; index++) {
    sC[index]=index;
    /* Create a new consumer */
    pthread_create(&idC, NULL, Consumer, &sC[index]);
  }

  pthread_exit(NULL);
}

