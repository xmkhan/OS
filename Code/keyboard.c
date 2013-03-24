#include "keyboard.h"
#include "process.h"
#include "memory.h"
#include "timer.h"
#include "message.h"
#include "crt_display.h"

char KEYBOARD_INPUT_BUFFER[64];
int curr_key_buf_pos = 0;
int WALL_CLOCK_RUNNING = 0;

//MSG *key_msg = (void *)0;
//MSG *command_msg = (void *)0;
//MSG *error_msg = (void *)0;

int COMMAND_PIDS[20] = {0};
char COMMAND_CHARS[20];
int command_pos = 0;

Process keyboard_process;
PCB* keyboard_pcb;

volatile char input_display[3];

//static PCB *saved_process = (void *)0;

void keyboard_init(void) {
  uint32_t *sp = (void *)0;
  int j = 0;
  
  // initialize keyboard display process
  keyboard_pcb = k_request_memory_block();
  keyboard_process.stack = k_request_memory_block();
  
  keyboard_pcb->pid = KEYBOARD_PID;
  keyboard_pcb->priority = 99;
  keyboard_pcb->type = INTERRUPT;
  keyboard_pcb->state = NEW;
  keyboard_pcb->head = (void *) 0;
  keyboard_pcb->next = (void *) 0;
  keyboard_process.pcb = keyboard_pcb;
  keyboard_process.start_loc = (uint32_t)keyboard_proc;
     
  sp = (uint32_t *)((uint32_t)keyboard_process.stack + MEMORY_BLOCK_SIZE);
    
  /* 8 bytes alignement adjustment to exception stack frame */
  if (!(((uint32_t)sp) & 0x04)) {
      --sp;
  }
    
  *(--sp)  = 0x01000000;              /* user process initial xPSR */ 
  *(--sp)  = keyboard_process.start_loc ;  /* PC contains the entry point of the process */

  for (j=0; j < 6; j++) {             /* R0-R3, R12 are cleared with 0 */
    *(--sp) = 0x0;
  }  
  keyboard_process.pcb->mp_sp = (uint32_t *)sp;
  
  insert_process_pq(keyboard_process.pcb);
}

void keyboard_proc(char input, PCB *saved_process)
{
	volatile char command[2];
	volatile int i = 0;
	
	volatile char *b = KEYBOARD_INPUT_BUFFER;
	MSG *key_msg = (void *)0;
	MSG *reg_msg = k_get_message(keyboard_pcb);
	MSG *command_msg = (void *)0;
		
	while (reg_msg != (void *)0) {
		
		if (reg_msg != 0 && reg_msg->msg_type == 4) {
			COMMAND_PIDS[command_pos] = reg_msg->sender_pid;
			COMMAND_CHARS[command_pos] = *((char *)reg_msg->msg_data);
			command_pos++;
			k_release_memory_block((void *)reg_msg);
		}
		reg_msg = k_get_message(keyboard_pcb);
	}
	
	if ((input == 13 && NUM_MEMORY_BLOCKS < 1) || (input != 13 && NUM_MEMORY_BLOCKS < 2)) {
		return;
	}
	key_msg = (MSG *)k_request_memory_block();
	// In case, request_mem_blk returns null
	if (key_msg == (void *)0) {
		return;
	}
	key_msg->msg_type = 1;
	
	// store input char in buffer, or process buffer if Enter is pressed
	if (input != 13) {
		KEYBOARD_INPUT_BUFFER[curr_key_buf_pos++] = input;
		input_display[0] = input;
		input_display[1] = '\0';
		key_msg->msg_data = (char *)input_display;
		k_send_message(CRT_PID, key_msg);
//		k_crt_i_process();
		return;
	}
	else {
		KEYBOARD_INPUT_BUFFER[curr_key_buf_pos++] = '\0';
		input_display[0] = '\n';
		input_display[1] = '\r';
		input_display[2] = '\0';
		key_msg->msg_data = (char *)input_display;
		k_send_message(CRT_PID, key_msg);
	//	k_crt_i_process();
		curr_key_buf_pos = 0;
	}
	
	// respond to commands that start with % only
	if (*b != '%') {
		return;
	}
	
	// now read in the type of command
	b++;
	while (*b != ' ' && *b != '\0') {
		command[i] = *b;
		b++;
		i++;
	}
	
	// skip over %
	b = KEYBOARD_INPUT_BUFFER+1;
	command_msg = (MSG *)k_request_memory_block();
	command_msg->msg_type = 1;
	command_msg->msg_data = (void *)b;
	
	// handle command types
	for (i = 0; i < command_pos; i++) {
		if (command[0] == COMMAND_CHARS[i]) {
			k_send_message(COMMAND_PIDS[i], command_msg);
			break;
		}
	}
}
