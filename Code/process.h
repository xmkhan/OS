#ifndef PROCESS_H
#define PROCESS_H

#include <LPC17xx.h>
#include "uart_polling.h"


#ifndef __SVC_0
#define __SVC_0  __svc_indirect(0)
#endif

#define NUM_PRIORITIES 4

typedef struct MSG MSG;

/** Process STATES */
typedef enum { NEW=0, RDY, RUN, BLKD, INTERRUPTED, EXIT } STATE;
typedef enum { USER=0, INTERRUPT } TYPE;
typedef enum { NONE=0, MEM_BLKD, MSG_BLKD } IO_STATUS;

/**
 * PCB struct containing relevent information for process management
 */
typedef struct PCB {
  unsigned int pid;
  uint8_t priority;
  TYPE type;
  STATE state;
  IO_STATUS status;
  uint32_t *mp_sp; /* stack pointer of the process */
	struct PCB *next;
  MSG *head; /* MSG_QUEUE (mailbox) */
} PCB;

/**
 * Process struct containing a PCB and other possible
 */
typedef struct Process {
  PCB *pcb;
  uint32_t start_loc;
  uint32_t *stack;
} Process;

/**
 * To other OS modules, exposes the current_process
 */
extern PCB *current_process;

void process_init(void);
int insert_process_pq(PCB* pcb);
int remove_process_pq(PCB* pcb);
PCB *lookup_pid(int pid);

/**
 * API: release_processor
 * Call for context switching, switches to next RDY process in line
 */
extern int k_context_switch(PCB* process);
#define context_switch(process) _context_switch((unsigned int)k_context_switch, process)
extern int _context_switch(unsigned int p_func, PCB *process) __SVC_0;

extern int k_release_processor(void);
#define release_processor() _release_processor((unsigned int)k_release_processor)
extern int _release_processor(unsigned int p_func) __SVC_0;

/**
 * API: set_process_priority
 * Set the priority level of a given process (PCB)
 */
extern int k_set_process_priority(int process_ID, int priority);
#define set_process_priority(process_ID, priority) _set_process_priority((unsigned int)k_set_process_priority, process_ID, priority)
extern int _set_process_priority(unsigned int p_func, int process_ID, int priority) __SVC_0;

/**
 * API: get_process_priority
 * Get the priority priority of a given process with its identifier
 */
extern int k_get_process_priority(int process_ID);
#define get_process_priority(process_ID) _get_process_priority((unsigned int)k_get_process_priority, process_ID)
extern int _get_process_priority(unsigned int p_func, int process_ID) __SVC_0;

extern void __rte(void);

#endif
