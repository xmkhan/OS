#ifndef PROCESS_H
#define PROCESS_H

#include "uart_polling.h"

#ifndef __SVC_0
#define __SVC_0  __svc_indirect(0)
#endif
#ifdef DEBUG
#define USR_SZ_STACK 0x200         /* user proc stack size 2048B = 0x200*4 */
#else
#define USR_SZ_STACK 0x080         /* user proc stack size 512B  = 0x80*4  */
#endif /* DEBUG */


typedef enum { NEW=0, READY, RUNNING, BLOCKED } STATE;

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
  uint32_t stack[USR_SZ_STACK];
  
  struct Process* next;
} Process;


void process_init(void);

extern int k_release_processor(void);
#define release_processor() _release_processor((unsigned int)k_release_processor)
extern int _release_processor(unsigned int p_func) __SVC_0;

extern int k_set_process_priority(void);
#define set_process_priority() _set_process_priority((unsigned int)k_set_process_priority)
extern int _set_process_priority(unsigned int p_func) __SVC_0;

extern int k_get_process_priority(void);
#define get_process_priority() _get_process_priority((unsigned int)k_get_process_priority)
extern int _get_process_priority(unsigned int p_func) __SVC_0;

#endif
