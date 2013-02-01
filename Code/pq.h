#ifndef PQ_H
#define PQ_H

#include "Process.h"
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

#endif
