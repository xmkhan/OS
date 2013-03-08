#include "uart_i_process.h"
#include "crt_display.h"
#include "usr_proc.h"
#include "message.h"
#include "process.h"
#include "memory.h"
#include "pq.h"

#define NUM_STATES  5
#define COL_SIZE    10 
#define BUFFER_SIZE 10

static char buffer[BUFFER_SIZE];
Process crt_process;
PCB* crt_pcb;

void crt_init(void) {
  uint32_t *sp = (void *)0;
  int j = 0;
  
  // initialize crt display process
  crt_pcb = k_request_memory_block();
  crt_process.stack = k_request_memory_block();
  
  crt_pcb->pid = CRT_PID;
  crt_pcb->priority = 1;
  crt_pcb->type = INTERRUPT;
  crt_pcb->state = NEW;
  crt_pcb->next = (void *) 0;
  crt_process.pcb = crt_pcb;
  crt_process.start_loc = (uint32_t) crt_interrupt;
     
  sp = (uint32_t *)((uint32_t)crt_process.stack + MEMORY_BLOCK_SIZE);
    
  /* 8 bytes alignement adjustment to exception stack frame */
  if (!(((uint32_t)sp) & 0x04)) {
      --sp;
  }
    
  *(--sp)  = 0x01000000;              /* user process initial xPSR */ 
  *(--sp)  = crt_process.start_loc ;  /* PC contains the entry point of the process */

  for (j=0; j < 6; j++) {             /* R0-R3, R12 are cleared with 0 */
    *(--sp) = 0x0;
  }  
  crt_process.pcb->mp_sp = (uint32_t *)sp;
  
  insert_process_pq(crt_process.pcb);

}

/**
 * System function available to processes
 * for non-blocking output to console
 * @param m - the message to be printed
 */
void crt_proc(char* m)
{
  volatile int length = 0;
  char* b = m;
  
  if(!b) return;
  
  //Find the length of the message
  while(*b != '\0') {
    b++;
    length++;
  }
  
  if(length == 0) return;
  
  //non-blocking output
  uart_i_process( 0, (uint8_t* ) m, length );
}

/**
 * Convert a positive integer or 0 to char*
 * @param buffer - stores char* representation
 *        input  - int to be converted
 */
void int_to_char_star(int input, volatile char* b) {
  
  //case for 0;
  if(input == 0) {
    *b = '0';
    b++;
  }
  
  //case for positive number
  while(input > 0) {
    *b = (char)(((int)'0')+input%10);
    input /= 10;
    b++;
  }
  
  //add null terminating character
  *b   = '\0';
}

void crt_output_int(int input) {
  int_to_char_star(input, buffer);
  crt_proc(buffer);
}

void crt_interrupt(void) {
  int sender_ID;
  MSG *msg = (void*) 0;
  
  msg = (MSG*) receive_message(&sender_ID);
  crt_proc(msg->msg_data);
  
  k_release_memory_block(msg);
}

/*
 * handle hot-key key press 
 */
void hot_key_handler(void) {
  char* header = "\n\rProc_id  Priority Status\n\r";
  int i = 0, j=0, n = 0, col_empty = 0;
  PCB *iterate;  
  
  //States' char* representation
  char* p_states[NUM_STATES];
  p_states[j++] = "New";
  p_states[j++] = "Ready";
  p_states[j++] = "Running";
  p_states[j++] = "Blocked";
  p_states[j++] = "Exit"; 

  //the categories we would be outputing
  crt_proc((void*)header);
  
  //iterate through every process
  //and output relevant information
  for(;i < NUM_PROCESSES; i++) {
    iterate = pcb_list[i];
    
    //output the process id
    int_to_char_star(i, buffer);
    n = i/10 + 1;
    col_empty = COL_SIZE - n;
    for(;n < col_empty; n++) {
      buffer[n] = ' ';
    }
    crt_proc((void*)buffer);
    
    //output the priority
    int_to_char_star(iterate->priority, buffer);
    n = iterate->priority/10 + 1;
    col_empty = COL_SIZE - n;
    for(;n < col_empty; n++) {
      buffer[n] = ' ';
    }
    
    //output the state
    crt_proc((void*)buffer);
    crt_proc((void*) p_states[iterate->state]);
    
    //Print a new line at the end
    buffer[0] = '\n';
    buffer[1] = '\r';
    buffer[2] = '\0';
    crt_proc((void*) buffer);
  }
}

