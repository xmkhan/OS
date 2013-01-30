#include "process.h"

#define NUM_PRIORITIES 4

#ifndef NULL
#define NULL (void *)0
#endif

static Process *p_pq[NUM_PRIORITIES];

static Process *current_process = NULL;

static unsigned int pid = 0;

int k_release_processor(void) {
	return 0;
}


int insert_pq(Process* p) {
  Process *pr_head = NULL;
    if (p == NULL || p->pcb == NULL || 
      p->pcb->priority >= NUM_PRIORITIES) {
        return -1;
    }
    
    pr_head = p_pq[p->pcb->priority];
    if (pr_head == NULL){
      p_pq[p->pcb->priority] = p;
    } else {
      while(pr_head->next != NULL)
      {
        pr_head = pr_head->next;
      }
      pr_head->next = p;
    }
    
    p->pcb->state = READY;
    
    return 0;
}

int remove_pq(Process* p) {
  Process *pr_head = NULL;
    if (p == NULL || p->pcb == NULL || 
      p->pcb->priority >= NUM_PRIORITIES) {
        return -1;
    }
    
    pr_head = p_pq[p->pcb->priority];
    if (pr_head != p)
    {
      while(pr_head->next != p) {
          pr_head = pr_head->next;
      }
      pr_head->next = p->next;
    } else
    {
      p_pq[p->pcb->priority] = pr_head->next;
      pr_head->pcb->state = EXIT;
    }
  return 0;
}

int scheduler(void) {
  unsigned int i = 0;
  Process *pr_head = NULL;
  for(; i < NUM_PRIORITIES; ++i)
  {
     pr_head = p_pq[i];
     while (pr_head != NULL && pr_head->pcb->state != READY)
     {
       pr_head = pr_head->next;
     }
     if (pr_head != NULL) return pr_head->pcb->pid;
  }
  return 0; // return NULL Process PID
}

Process *lookup_pid(int pid) {
  unsigned int i = 0;
  Process *pr_head = NULL;
  for(; i < NUM_PRIORITIES; ++i)
  {
     pr_head = p_pq[i];
     while (pr_head != NULL && pr_head->pcb->pid != pid)
     {
       pr_head = pr_head->next;
     }
     if (pr_head != NULL) return pr_head;
  }
  return (void *)0;
}

void process_init(void) {
  unsigned int i = 0;
  for (; i < NUM_PRIORITIES; ++i)
  {
    p_pq[i] = (void *)0;
  }
}
