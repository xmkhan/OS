#include "semaphore.h"
#include "pq.h"

void semInit(semaphore *s) {
  s->count = 1;
  s->queue = (void *)0;
}

void semWait(semaphore *s) {
  s->count--;
  if (s->count < 0) { 
    current_process->state = BLKD;
    current_process->status = SEM_BLKD;
    remove_process_pq(current_process);
    enqueue_q(&(s->queue), current_process, PCB_T);
    k_release_processor();
  }
}

void semSignal(semaphore *s) {
  s->count++;
  if ( s->count <= 0) {
    PCB *p = dequeue_q(&(s->queue), PCB_T);
    p->state = RDY;
    p->status = NONE;
    insert_process_pq(p);
  }
}
