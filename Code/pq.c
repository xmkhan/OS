#include "pq.h"

#ifndef NULL
#define NULL (void *)0
#endif

int insert_pq(Process* pq[], Process* p) {
  volatile Process *pr_head = NULL;
    if (p == NULL || p->pcb == NULL || 
      p->pcb->priority >= NUM_PRIORITIES) {
        return -1;
    }
    
    pr_head = pq[p->pcb->priority];
    if (pr_head == NULL){
      pq[p->pcb->priority] = p;
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

int remove_pq(Process* pq[], Process* p) {
  volatile Process *pr_head = NULL;
  if (p == NULL || p->pcb == NULL || 
    p->pcb->priority >= NUM_PRIORITIES) {
      return -1;
  }
    
  pr_head = pq[p->pcb->priority];
  if (pr_head != p)
  {
    while(pr_head->next != p) {
        pr_head = pr_head->next;
    }
    pr_head->next = p->next;
  } else
  {
    pq[p->pcb->priority] = pr_head->next;
    //pr_head->pcb->state = EXIT;
  }
  return 0;
}

Process* get_process(Process *pq[]) {
  volatile unsigned int i = 0;
  volatile Process *pr_head = NULL;
  for(; i < NUM_PRIORITIES; ++i)
  {
     pr_head = pq[i];
     while (pr_head != NULL)
     {
       pr_head = pr_head->next;
     }
     if (pr_head != NULL) return (Process*) pr_head;
  }
  return 0; // return NULL
}
