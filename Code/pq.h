#ifndef PQ_H
#define PQ_H

#include "Process.h"

int insert_pq(PCB* pq[], PCB* p);
int remove_pq(PCB* pq[], PCB* p);
PCB *get_process(PCB* pq[]);
PCB *lookup_pid_pq(PCB* pq[], int pid);

#endif
