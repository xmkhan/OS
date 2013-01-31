#ifndef PQ_H
#define PQ_H

#include "Process.h"

int insert_pq(Process* pq[], Process* p);
int remove_pq(Process* pq[], Process* p);
Process* get_process(Process* pq[]);
Process *lookup_pid_pq(Process* pq[], int pid);

#endif
