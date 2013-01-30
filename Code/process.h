#ifndef PROCESS_H
#define PROCESS_H

#include <LPC17xx.h>
#include "uart_polling.h"

#ifndef __SVC_0
#define __SVC_0  __svc_indirect(0)
#endif


typedef enum { NEW=0, RDY, RUN, BLKD, EXIT } STATE;
typedef enum { NONE=0, MEM } EVENT_Q;

typedef struct PCB {
  unsigned int pid;
  uint8_t priority;
  STATE state;
  uint32_t *mp_sp; /* stack pointer of the process */
  uint8_t importance;
} PCB;

typedef struct Process {
  PCB *pcb;
  uint32_t start_loc;
  uint32_t *stack;
  struct Process* next;
} Process;

extern Process *current_process;

void process_init(void);



extern int k_release_processor(void);
#define release_processor() _release_processor((unsigned int)k_release_processor)
extern int _release_processor(unsigned int p_func) __SVC_0;

extern int k_set_process_priority(int process_ID, int priority);
#define set_process_priority() _set_process_priority((unsigned int)k_set_process_priority, process_ID, priority)
extern int _set_process_priority(unsigned int p_func, int process_ID, int priority) __SVC_0;

extern int k_get_process_priority(int process_ID);
#define get_process_priority() _get_process_priority((unsigned int)k_get_process_priority, process_ID)
extern int _get_process_priority(unsigned int p_func, int process_ID) __SVC_0;

extern void __rte(void);

#endif
