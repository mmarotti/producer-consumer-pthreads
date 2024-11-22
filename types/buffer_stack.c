#include <semaphore.h>

#include "../definitions.h"

typedef struct sbuf_t {
  sbuf_s *buf[BUFF_SIZE];   /* shared var */
  int in;                  /* buf[in%BUFF_SIZE] is the first empty slot */
  int out;                 /* buf[out%BUFF_SIZE] is the first full slot */
  sem_t *full;              /* keep track of the number of full spots */
  sem_t *empty;             /* keep track of the number of empty spots */
  sem_t *mutex;             /* enforce mutual exclusion to shared data */
} sbuf_t;

sbuf_t shared[BUFF_NUMBER];          /* two shared buffers */