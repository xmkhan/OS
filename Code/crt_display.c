#include "uart_i_process.h"
#include "crt_display.h"
#include "usr_proc.h"
#include "message.h"
#include "process.h"
#include "memory.h"
#include "pq.h"

#define NUM_STATES  8
#define COL_SIZE    10 
#define BUFFER_SIZE 10

static char buffer[BUFFER_SIZE];
Process crt_process;
PCB* crt_pcb;
Process hotkey_process;
PCB* hotkey_pcb;
MSG* hotkey_msg;

void hotkey_init(void) {
  uint32_t *sp = (void *)0;
  int j = 0;
  
  // initialize hotkey display process
  hotkey_pcb = k_request_memory_block();
  hotkey_process.stack = k_request_memory_block();
  
  hotkey_pcb->pid = HOTKEY_PID;
  hotkey_pcb->priority = 99;
  hotkey_pcb->type = USER;
  hotkey_pcb->state = NEW;
  hotkey_pcb->status = NONE;
  hotkey_pcb->head = (void *) 0;
  hotkey_pcb->next = (void *) 0;
  hotkey_process.pcb = hotkey_pcb;
  hotkey_process.start_loc = (uint32_t)hot_key_handler;
  
  sp = (uint32_t *)((uint32_t)hotkey_process.stack + MEMORY_BLOCK_SIZE);
    
  /* 8 bytes alignement adjustment to exception stack frame */
  if (!(((uint32_t)sp) & 0x04)) {
      --sp;
  }
    
  *(--sp)  = 0x01000000;              /* user process initial xPSR */ 
  *(--sp)  = hotkey_process.start_loc ;  /* PC contains the entry point of the process */

  for (j=0; j < 6; j++) {             /* R0-R3, R12 are cleared with 0 */
    *(--sp) = 0x0;
  }  
  hotkey_process.pcb->mp_sp = (uint32_t *)sp;
  
  insert_process_pq(hotkey_process.pcb);
  hotkey_msg = (MSG*) k_request_memory_block();
	hotkey_msg->msg_type = 2;
  hotkey_msg->msg_data = (void*) k_request_memory_block();
}

void crt_init(void) {
  uint32_t *sp = (void *)0;
  int j = 0;
  
  // initialize crt display process
  crt_pcb = k_request_memory_block();
  crt_process.stack = k_request_memory_block();
  
  crt_pcb->pid = CRT_PID;
  crt_pcb->priority = 0;
  crt_pcb->type = SYSTEM;
  crt_pcb->state = NEW;
  crt_pcb->status = NONE;
  crt_pcb->head = (void *) 0;
  crt_pcb->next = (void *) 0;
  crt_process.pcb = crt_pcb;
  crt_process.start_loc = (uint32_t) crt_system_process;
     
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

#define def_crt_print(kprefix) void kprefix##crt_print(char *input) {\
	MSG* msg = (void *)0;\
  int iState = kprefix##get_interrupt_state();\
  kprefix##set_interrupt_state(4);\
	msg = (MSG*)kprefix##request_memory_block();\
  msg->msg_data = (void*) input;\
  msg->msg_type = 1;\
  kprefix##send_message(CRT_PID, msg);\
  kprefix##set_interrupt_state(iState);\
}

def_crt_print();
def_crt_print(k_);

/*
void crt_print(char* input) {
  PCB* saved_process = current_process;
	MSG* msg = (void *)0;
	if (__get_CONTROL() == BIT(0)) {
		msg = (MSG*) request_memory_block();
	}
	else {
		msg = (MSG*) k_request_memory_block();
	}
	
  msg->msg_data = (void*) input;
  msg->msg_type = 1;
  
	if (__get_CONTROL() == BIT(0)) {
		send_message(CRT_PID, msg);
	}
	else {
		k_send_message(CRT_PID, msg);
	}
  
  if(!(saved_process->pid == 0 && saved_process->state == NEW))
  if (__get_CONTROL() == BIT(0)) {  
		context_switch(crt_pcb);
	}
	else {
		k_context_switch(crt_pcb);
	}
  
  //crt_i_process();
  
  if(!(saved_process->pid == 0 && saved_process->state == NEW))
  if (__get_CONTROL() == BIT(0)) {  
		context_switch(saved_process);
	}
	else {
		k_context_switch(saved_process);
	}
}
*/

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
  crt_print(buffer);
}

void crt_system_process(void) {	
  volatile int length = 0, sender_pid = 0;
  char* b = (void*) 0;
  MSG *msg = (void*) 0;
  
  while(1)
  {
    msg = (MSG*) receive_message((int*)&sender_pid);
	
    b = msg->msg_data;
    length = 0;
    if(b)
    {
      //Find the length of the message
      while(*b != '\0') {
        b++;
        length++;
      }
      
      if(length != 0)
      {
        //non-blocking output
        uart_i_process( 0, (uint8_t* ) msg->msg_data, length );
      }
    }
  
    if(msg->msg_type != 2)
    {
      release_memory_block((void *)msg);
    }
  }
}

void concat (char** mem_block, char* print)
{
  char* temp = print;
  while(*temp != '\0')
  {
    *((char *)(*mem_block)) = *temp;
    temp++;
    (*mem_block) = (*mem_block) + 1;
  }
}


/*
 * handle hot-key key press 
 */
void hot_key_handler(void) {
	while (1) {
	MSG *msg = (void *)0;
	int sender_id = 0;
  char* out = (char*) hotkey_msg->msg_data;
  char* header = "";
  int i = 0, j=0, n = 0, col_empty = 0;
  PCB *iterate;  
	PCB *saved_process;
  
  //States' char* representation
  char* p_states[NUM_STATES];
 	msg = (MSG *)receive_message(&sender_id);
	saved_process = (PCB *)msg->msg_data;
	context_switch(saved_process);

  p_states[j++] = "New";
  p_states[j++] = "Ready";
  p_states[j++] = "Running";
  p_states[j++] = "Message BLKD";
  p_states[j++] = "Interrupted"; 
	p_states[j++] = "Exit"; 
  p_states[j++] = "Memory BLKD";
  p_states[j++] = "Message Semaphore BLKD";

  //the categories we would be outputing
  concat(&out, header);
//  crt_print((void*)header);
  
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
    concat(&out, buffer);
    //crt_print((void*)buffer);
    
    //output the priority
    int_to_char_star(iterate->priority, buffer);
    n = iterate->priority/10 + 1;
    col_empty = COL_SIZE - n;
    for(;n < col_empty; n++) {
      buffer[n] = ' ';
    }
    
    //output the state
    concat(&out, buffer);
    //crt_print((void*)buffer);
    if(iterate->state == BLKD) {
      if(iterate->status == MEM_BLKD) {
        concat(&out, p_states[6]);
        //crt_print((void*) p_states[6]);
      }
      else if(iterate->status == MSG_BLKD) {
        concat(&out, p_states[3]);
//        crt_print((void*) p_states[3]);
      }
      else if(iterate->status == SEM_BLKD) {
          concat(&out, p_states[7]);
//          crt_print((void*) p_states[7]);
      }
    }
    else {
      concat(&out, p_states[iterate->state]);
//      crt_print((void*) p_states[iterate->state]);
    }
    
    //Print a new line at the end
    buffer[0] = '\n';
    buffer[1] = '\r';
    buffer[2] = '\0';
    concat(&out, buffer);
//    crt_print((void*) buffer);
    
  }
  *out = '\0';
  send_message(CRT_PID, hotkey_msg);
	msg = (MSG *)receive_message(&sender_id);
	saved_process = (PCB *)msg->msg_data;
	context_switch(saved_process);
	}
}

