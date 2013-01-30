#include "process.h"
#include "memory.h"
#include "uart_polling.h"
#include "usr_proc.h"

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif  /* DEBUG */

#define INITIAL_xPSR 0x01000000

PCB *pcb_list[NUM_PROCESSES];
Process process_list[NUM_PROCESSES];

typedef void (*process_ptr)(void);

void __initialize_processes(void) {
  uint32_t *sp = (void *)0;
  volatile unsigned int i = 0, j = 0, k = 0;  
    
  process_ptr process_t[] = {null_process, proc1, proc2};
  int priority_t[] = {3, 1, 1};

  for(k=0;k< NUM_PROCESSES; k++) {
    pcb_list[k] = k_request_memory_block();
    process_list[k].stack = k_request_memory_block();
    printf(" %x %x\n", pcb_list[k], process_list[k].stack);
  }

  for (; i < NUM_PROCESSES; i++) {
    pcb_list[i]->pid = i;
    pcb_list[i]->priority = priority_t[i];
    pcb_list[i]->state = NEW;
    process_list[i].pcb = (PCB *)&pcb_list[i];
    process_list[i].start_loc = (uint32_t)process_t[i];
    
    sp = (uint32_t *)((uint32_t)process_list[i].stack + MEMORY_BLOCK_SIZE_HEX);;
    
    /* 8 bytes alignement adjustment to exception stack frame */
    if (!(((uint32_t)sp) & 0x04)) {
        --sp;
    }
    
    *(--sp)  = INITIAL_xPSR;      /* user process initial xPSR */ 
    *(--sp)  = (uint32_t)process_t[i];  /* PC contains the entry point of the process */

    for (j=0; j < 6; j++) { /* R0-R3, R12 are cleared with 0 */
      *(--sp) = 0x0;
    }
    
    process_list[i].pcb->mp_sp = (uint32_t *)sp;
  }
}

void null_process(void) {
  while(1) {
    volatile int ret_val = release_processor();
#ifdef DEBUG
      printf("\n\rnull_proc: ret_val=%d. ", ret_val);
#endif
  }
}

void proc1(void)
{
  volatile int i =0;
  volatile int ret_val = 10;
  while ( 1) {
    if (i!=0 &&i%5 == 0 ) {
      ret_val = release_processor();
#ifdef DEBUG
      printf("\n\rproc1: ret_val=%d. ", ret_val);
#else
      uart0_put_string("\n\r");
#endif
    }
    uart0_put_char('A' + i%26);
    i++;
  }
}

void proc2(void)
{
  volatile int i =0;
  volatile int ret_val = 20;
  while ( 1) {
    if (i!=0 &&i%5 == 0 ) {
      ret_val = release_processor();
#ifdef DEBUG
      printf("\n\rproc2: ret_val=%d. ", ret_val);
#else
      uart0_put_string("\n\r");
#endif
    }
    uart0_put_char('a' + i%26);
    i++;
  }
}
