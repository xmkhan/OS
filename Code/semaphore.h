#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "process.h"

typedef struct semaphore {
  int count;
  PCB *queue;
} semaphore;

void semInit(semaphore *s);
void semWait(semaphore *s);
void semSignal(semaphore *s);

#endif
