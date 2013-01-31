#include "pq.h"

#ifndef NULL
#define NULL (void *)0
#endif

int insert_pq(PCB* pq[], PCB* p) {
  volatile PCB *pr_head = NULL;
    if (p == NULL || p->priority >= NUM_PRIORITIES) {
        return -1;
    }
    
    pr_head = pq[p->priority];
    if (pr_head == NULL){
      pq[p->priority] = p;
    } else {
      while(pr_head->next != NULL)
      {
        pr_head = pr_head->next;
      }
      pr_head->next = p;
      p->next = NULL;
    }    
    return 0;
}

int remove_pq(PCB* pq[], PCB* p) {
  volatile PCB *pr_head = NULL;
  if (p == NULL || p->priority >= NUM_PRIORITIES) {
      return -1;
  }
    
  pr_head = pq[p->priority];
  if (pr_head != p)
  {
    while(pr_head->next != p) {
        pr_head = pr_head->next;
    }
    pr_head->next = p->next;
  } else
  {
    pq[p->priority] = pr_head->next;
    //pr_head->pcb->state = EXIT;
  }
  return 0;
}

PCB* get_process(PCB *pq[]) {
  volatile unsigned int i = 0;
  volatile PCB *pr_head = NULL;
  for(; i < NUM_PRIORITIES; ++i)
  {
     pr_head = pq[i];
     while (pr_head != NULL)
     {
       pr_head = pr_head->next;
     }
     if (pr_head != NULL) return (PCB*) pr_head;
  }
  return 0; // return NULL
}

PCB *lookup_pid_pq(PCB* pq[], int pid) {
  volatile unsigned int i = 0;
  volatile PCB *pr_head = NULL;
  for(; i < NUM_PRIORITIES; ++i)
  {
     pr_head = pq[i];
     while (pr_head != NULL && pr_head->pid != pid)
     {
       pr_head = pr_head->next;
     }
     if (pr_head != NULL) return (PCB *)pr_head;
  }
  return (void *)0;
}
