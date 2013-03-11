#include "process.h"
#include "memory.h"
#include "message.h"
#include "usr_proc.h"
#include "crt_display.h"

#define INITIAL_xPSR 0x01000000

// test state variables
int TEST1 = 0;
int TEST2 = 0;
int TEST3 = 0;
int TEST4 = 0;
int TEST5 = 0;
int TEST6 = 0;
int TEST7 = 0;
int TESTEND = 0;
int NUM_TESTS_PASSED = 0;
int NUM_TESTS_FAILED = 0;

// process list
PCB *pcb_list[NUM_PROCESSES];
Process process_list[NUM_PROCESSES];

typedef void (*process_ptr)(void);

// set up each process (including null) and their stack frames
void __initialize_processes(void) {
  uint32_t *sp = (void *)0;
  volatile unsigned int i = 0, j = 0, k = 0;  
  
  process_ptr process_t[] = {null_process, proc1, proc2, proc3, proc4, proc5, proc6};
  int priority_t[] = {4, 0, 0, 1, 1, 1, 1};

  for(k = 0; k < NUM_PROCESSES; k++) {
    pcb_list[k] = k_request_memory_block();
    process_list[k].stack = k_request_memory_block();
  }

  for (; i < NUM_PROCESSES; i++) {
    pcb_list[i]->pid = i;
    pcb_list[i]->priority = priority_t[i];
    pcb_list[i]->type = USER;
    pcb_list[i]->state = NEW;
    pcb_list[i]->head = (void *)0;
    process_list[i].pcb = (PCB *)pcb_list[i];
    process_list[i].start_loc = (uint32_t)process_t[i];
    process_list[i].pcb->next = (void *)0;
    
    sp = (uint32_t *)((uint32_t)process_list[i].stack + MEMORY_BLOCK_SIZE);
    
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

// null process: simply release processor
void null_process(void) {
  
  while(1) {
    volatile int ret_val = release_processor();
  }
}

// process 1: Testing for message sending
void proc1(void)
{
  while(1)
  {
    volatile int status = 10;
    volatile int ret_val = 10;
    volatile int send_status = 10;
    int x1 = 10, x2 = 20;
    volatile MSG *msg = (volatile MSG *) request_memory_block();
    volatile MSG *msg2 = (volatile MSG *) request_memory_block();
    
    msg->msg_data = (void *) &x1;
    msg->msg_type = 1;
    send_status = send_message(2, (MSG *)msg);
    
    msg2->msg_data = (void *) &x2;
    msg2->msg_type = 1;
    send_status = send_status  | send_message(2, (MSG *)msg2);
    
    if (send_status == 0) {
      crt_print("G013_test: test 1 OK\n\r");
    } else {
      crt_print("G013_test: test 1 FAIL\n\r");
    }
    ret_val = release_processor();
  }
}

// process 2: Testing for message receiving
void proc2(void)
{
  while(1)
  {
    volatile int ret_val = 10;
    int sender_pid = -1;
    int pass = 0;
    volatile MSG *msg = receive_message(&sender_pid);
    volatile MSG *msg2 = receive_message(&sender_pid);
    if (*((int *)msg->msg_data) == 10 && *((int *)msg2->msg_data) == 20) {
      pass++;
      ret_val = release_memory_block((void *)msg);
      ret_val = ret_val | release_memory_block((void *)msg2);
    }

    if (ret_val == 0) {
      pass++;
    }
    if (pass == 2) {
      crt_print("G013_test: test 2 OK\n\r");
    } else {
      crt_print("G013_test: test 2 FAIL\n\r");
    }
    ret_val = release_processor();
  }
}

// process 3: Testing for multiple send/multiple receive
void proc3(void)
{
  while(1) {
  volatile int ret_val = 10;
  ret_val = release_processor();
 }
}

// process 4: set process priority, then block by requesting memory (already gone from proc1)
void proc4(void)
{
  while (1) {
  volatile int ret_val = 10;
  ret_val = release_processor();
  }
}

// process 5: test registers/general variable allocation and results of proc4
void proc5(void)
{
  while (1) {
  volatile int ret_val = 10;
  ret_val = release_processor();
  }
}

// process 6: end of tests, print results
void proc6(void)
{
	volatile int ret_val = 20;
  while (1) {
    ret_val = release_processor();
			if (!TESTEND) {
			TESTEND = 1;
		}
  }
}
