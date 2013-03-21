#include "memory.h"
#include "process.h"
#include "message.h"
#include "usr_proc.h"
#include "pq.h"
#include "timer.h"
#include "keyboard.h"
#include "crt_display.h"
#include "uart_i_process.h"
#include "wall_clock.h"

#ifdef DEBUG
#include <stdio.h>
#endif  /* DEBUG */

#ifndef NULL
#define NULL (void *)0
#endif

volatile int state = 7;

volatile static PCB *p_pq[NUM_PRIORITIES]; // PCB PQ
PCB *current_process;

int insert_process_pq(PCB* p) {
  return insert_pq((PCB**) p_pq, p);
}
int remove_process_pq(PCB* p) {
    return remove_pq((PCB**) p_pq, p);
}

/**
 * Select the next process to be scheduled
 * @return  PID
 */
int scheduler(void) {
  volatile unsigned int i = 0;
  volatile PCB *pr_head = NULL;
  for(; i < NUM_PRIORITIES; ++i)
  {
     pr_head = p_pq[i];
     while (pr_head != NULL && ((pr_head->state != RDY && pr_head->state != NEW) || pr_head->type == INTERRUPT))
     {
       pr_head = pr_head->next;
     }
     if (pr_head != NULL) return pr_head->pid;
  }
  return 0; // return NULL Process PID
}

/**
 * Look up PCB from its PID
 */
PCB *lookup_pid(int pid) {
  int i;
  for (i =0; i < NUM_PROCESSES; ++i) {
    if (pcb_list[i]->pid == pid) return pcb_list[i];
  }
  
	// check for special processes
	switch (pid) {
		case TIMER_PID:
			return timer_pcb;
		case CRT_PID:
			return crt_pcb;
		case WALL_CLOCK_PID:
			return wall_clock_pcb;
		case HOTKEY_PID:
			return hotkey_pcb;
	}
	return (void *)0;
}

/**
 * Get process priority
 */
int k_get_process_priority(int process_ID) {
  PCB *p = lookup_pid(process_ID);
  if (p == NULL) return -1;
  return p->priority;
}

/**
 * Set process priority
 */
int k_set_process_priority(int process_ID, int priority) {
  PCB *p = lookup_pid(process_ID);
  if (process_ID == 0 ||
    !(priority >= 0 && priority < NUM_PRIORITIES)) return -1; // don't change priority of null process
  p->priority = priority;
  return p->priority;
}

/**
 * Initializes the process module
 */
void process_init(void) {
  volatile unsigned int i = 0;
    __initialize_processes();
  for (; i < NUM_PROCESSES; ++i)
  {
    process_list[i].pcb->state = NEW;
    insert_process_pq(process_list[i].pcb);
  }

  current_process = process_list[0].pcb; // null process
  remove_process_pq((PCB*)current_process);
}

/**
 * Handles context switching and managing process STATE to next scheduled process
 * @return  [0 successful, -1 for error]
 */
int k_release_processor(void) {
 	 volatile int pid = scheduler();
   PCB* process = lookup_pid(pid);
   return k_context_switch(process);
}

/**
 * Handles context switching and managing process STATE to given pcb
 * @return  [0 successful, -1 for error]
 */
int k_context_switch(PCB* process) {
   PCB *old_process = current_process;
   volatile STATE state;
   int iState = k_get_interrupt_state();
   k_set_interrupt_state(0);
  
   current_process = process;

	 if (current_process == NULL) {
     k_set_interrupt_state(iState);
	   return -1;
	 }
	 state = current_process->state;

   if (state == NEW) {
	   if (old_process->state != NEW) {
       if(old_process->type != INTERRUPT)
          old_process->mp_sp = (uint32_t *) __get_MSP();

       if(old_process->state != BLKD) {
		     old_process->state = current_process->type == INTERRUPT ? INTERRUPTED : RDY;
         insert_process_pq((PCB*)old_process);
       }
     }
     remove_process_pq((PCB*)current_process);
		 current_process->state = RUN;
     current_process->status = NONE;
     
     if(current_process->type != INTERRUPT)
     {
        __set_MSP((uint32_t) current_process->mp_sp);
        k_set_interrupt_state(iState);
        __rte();  /* pop exception stack frame from the stack for a new process */
     }
	 } else if (state == RDY || state == INTERRUPTED) {
     if(old_process->type != INTERRUPT)
        old_process->mp_sp = (uint32_t *) __get_MSP(); /* save the old process's sp */

     if(old_process->state != BLKD)
     {
   		 old_process->state = current_process->type == INTERRUPT ? INTERRUPTED : RDY;
       insert_process_pq((PCB*)old_process);
     }
     remove_process_pq((PCB*)current_process);
		 current_process->state = RUN;
     current_process->status = NONE;
     
     if(current_process->type != INTERRUPT)
        __set_MSP((uint32_t) current_process->mp_sp); /* switch to the new proc's stack */
	 } else {
	     current_process = old_process; /* revert back to the old proc on error */
       k_set_interrupt_state(iState);
	     return -1;
	 }
  k_set_interrupt_state(iState);
	return 0;
}

int k_get_interrupt_state()
{
  return state;
}

void k_set_interrupt_state(int newState)
{
  LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;
  
  unsigned int timer = newState&1;
  unsigned int kb = newState&2;
  unsigned int crt = newState&4;
 
  
  switch(timer)
  {
    case 1:
      NVIC_EnableIRQ(TIMER0_IRQn);
      break;
    case 0:
      NVIC_DisableIRQ(TIMER0_IRQn);
      break;      
  }
  
  switch(kb) {
    case 2:
      pUart->IER = pUart->IER | IER_RBR | IER_RLS |  IER_THRE;  
      NVIC_EnableIRQ(UART0_IRQn);
      break;
    case 0:
      pUart->IER = pUart->IER & !(IER_RBR | IER_RLS | IER_THRE);  
      break;
  }
  
  switch(crt) {
    case 4:
      pUart->IER = pUart->IER | IER_RBR | IER_RLS |  IER_THRE;  
      NVIC_EnableIRQ(UART0_IRQn);
      break;
    case 0:
      pUart->IER = pUart->IER & !(IER_RBR | IER_RLS | IER_THRE);
      break;
  }
  
  state = newState;
}
