#include "process.h"

#define NUM_PRIORITIES 4

#ifndef NULL
#define NULL (void *)0
#endif

static Process *p_pq[NUM_PRIORITIES];
static Process *current_process = NULL;

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
    
    p->pcb->state = RDY;
    
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
     while (pr_head != NULL && pr_head->pcb->state != RDY)
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

int k_release_processor(void) {
   Process *old_process = current_process;
   volatile STATE state;
	 volatile int pid = scheduler();
   current_process = lookup_pid(pid);
  
	 if (current_process == NULL) {
	   return -1;  
	 }
	 state = current_process->pcb->state;
   
   if (state == NEW) {
	   if (old_process->pcb->state != NEW) {
		     old_process->pcb->state = RDY;
         old_process->pcb->mp_sp = (uint32_t *) __get_MSP();
		 }
		 current_process->pcb->state = RUN;
		 __set_MSP((uint32_t) current_process->pcb->mp_sp);
		 __rte();  /* pop exception stack frame from the stack for a new process */
	 } else if (state == RDY) {
		 old_process->pcb->state = RDY; 
		 old_process->pcb->mp_sp = (uint32_t *) __get_MSP(); /* save the old process's sp */
		 
		 current_process->pcb->state = RUN;
		 __set_MSP((uint32_t) current_process->pcb->mp_sp); /* switch to the new proc's stack */		
	 } else {
	     current_process = old_process; /* revert back to the old proc on error */
	     return -1;
	 } 
	return 0;
}
