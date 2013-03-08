#ifndef PQ_H
#define PQ_H

#include "process.h"
/**
 * Services as a higher order package for Priority Queue providing events and mechanisms
 * with general purpose ADT functions: insert, remove, lookup
 * @param  pq [Priority Queue]
 * @param  p  [Process Control Block]
 * @return    [status code on insert/remove, PCB on lookups]
 */
int insert_pq(PCB* pq[], PCB* p);
int remove_pq(PCB* pq[], PCB* p);
PCB *get_process(PCB* pq[]);
PCB *lookup_pid_pq(PCB* pq[], int pid);

// Handling generic PCB and MSG structures
typedef enum { PCB_T=0, MSG_T, DLY_MSG_T } q_type;

// Standard queue add/remove functions
int enqueue_q(void* pq, void* p, q_type type);
void *dequeue_q(void* pq, q_type type);

#endif
