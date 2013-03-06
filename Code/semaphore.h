#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "process.h"

// Semaphore data structure keeping track of count and blocking queue
typedef struct semaphore {
  int count;
  PCB *queue;
} semaphore;

/**
 * Initializes semaphore s to (count =0, queue=NULL)
 */
void semInit(semaphore *s);

/**
 * Lock/unlock mechanism through semaphores (review slide notes)
 */
void semWait(semaphore *s);
void semSignal(semaphore *s);

#endif
