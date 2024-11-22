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
#include "threads/consumer_producer_1.c"
#include "threads/consumer_producer_2.c"
#include "threads/consumer_producer_3.c"
#include "threads/consumer.c"

void create_semaphores() {
  char sem_name[32];
  for (int index = 0; index < BUFF_NUMBER; index++) {
    char sem_name[32];
    
    /* MacOS does not support unnamed semaphores, so we are using named ones */
    snprintf(sem_name, sizeof(sem_name), "/sem_full_%d",index);
    shared[index].full = sem_open(sem_name, O_CREAT, 0644, 0);
    snprintf(sem_name, sizeof(sem_name), "/sem_empty_%d", index);
    shared[index].empty = sem_open(sem_name, O_CREAT, 0644, BUFF_SIZE);
    snprintf(sem_name, sizeof(sem_name), "/sem_mutex_%d", index);
    shared[index].mutex = sem_open(sem_name, O_CREAT, 0644, 1);
  }
}

void cleanup_semaphores() {
  char sem_name[32];
  for (int index = 0; index < BUFF_NUMBER; index++) {
    snprintf(sem_name, sizeof(sem_name), "/sem_full_%d", index);
    sem_close(shared[index].full);
    sem_unlink(sem_name);

    snprintf(sem_name, sizeof(sem_name), "/sem_empty_%d", index);
    sem_close(shared[index].empty);
    sem_unlink(sem_name);

    snprintf(sem_name, sizeof(sem_name), "/sem_mutex_%d", index);
    sem_close(shared[index].mutex);
    sem_unlink(sem_name);
  }
}

int main() {
  int index;
  int sP[NP], sCP1[NCP1], sCP2[NCP2], sCP3[NCP3], sC[NC];
  pthread_t idP[NP], idCP1[NCP1], idCP2[NCP2], idCP3[NCP3], idC[NC];

  create_semaphores();

  for (index = 0; index < NP; index++) {
    sP[index] = index;
    /* Create a new producer */
    pthread_create(&idP[index], NULL, Producer, &sP[index]);
  }
  for (index = 0; index < NCP1; index++) {
    sCP1[index] = index;
    /* Create a new consumer producer 1 */
    pthread_create(&idCP1[index], NULL, ConsumerProducer1, &sCP1[index]);
  }
  for (index = 0; index < NCP2; index++) {
    sCP2[index] = index;
    /* Create a new consumer producer 2 */
    pthread_create(&idCP2[index], NULL, ConsumerProducer2, &sCP2[index]);
  }
  for (index = 0; index < NCP3; index++) {
    sCP3[index] = index;
    /* Create a new consumer producer 3 */
    pthread_create(&idCP3[index], NULL, ConsumerProducer3, &sCP3[index]);
  }
  for (index = 0; index < NC; index++) {
    sC[index] = index;
    /* Create a new consumer */
    pthread_create(&idC[index], NULL, Consumer, &sC[index]);
  }

  /* Wait for all threads to complete */
  for (index = 0; index < NP; index++) {
    pthread_join(idP[index], NULL);
  }
  for (index = 0; index < NCP1; index++) {
    pthread_join(idCP1[index], NULL);
  }
  for (index = 0; index < NCP2; index++) {
    pthread_join(idCP2[index], NULL);
  }
  for (index = 0; index < NCP3; index++) {
    pthread_join(idCP3[index], NULL);
  }
  for (index = 0; index < NC; index++) {
    pthread_join(idC[index], NULL);
  }

  cleanup_semaphores();

  return EXIT_SUCCESS;
}