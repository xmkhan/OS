#include "keyboard.h"
#include "process.h"
#include "memory.h"
#include "timer.h"
#include "message.h"
#include "crt_display.h"

char KEYBOARD_INPUT_BUFFER[64];
int curr_key_buf_pos = 0;
int WALL_CLOCK_RUNNING = 0;

MSG *key_msg = (void *)0;
MSG *error_msg = (void *)0;

int COMMAND_PIDS[20] = {0};
char COMMAND_CHARS[20];
int command_pos = 0;

Process keyboard_process;
PCB* keyboard_pcb;

//static PCB *saved_process = (void *)0;

int str2int(char *s) {
	char *t = s;
	int total = 0;
	int multiplier = 1;
	
	while (t) {
		t++;
	}
	t--;
	
	while (t != s) {
		total += ((*s) + 48) * multiplier;
		multiplier *= 10;
		t--;
	}
	return total;
}

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
	
	// Allocate memory for keyboard so that when memory runs out keyboard features used for debugging still work
  key_msg = (MSG*) k_request_memory_block();  
	key_msg->msg_type = 2;
	
	error_msg = (MSG *)k_request_memory_block();
	error_msg->msg_type = 1;
	error_msg->msg_data = "Invalid command.";
}

void keyboard_proc(char input, PCB *saved_process)
{
	volatile char command[2];
	volatile int i = 0;
	
	char input_display[3];
	char pid_str[4];
	int pid;
	char priority_str[4];
	int priority;
	
	volatile char *b = KEYBOARD_INPUT_BUFFER;
	MSG *reg_msg = k_get_message(keyboard_pcb);
	MSG *error_msg = (MSG *)k_request_memory_block();
	
	if (reg_msg->msg_type == 4) {
		COMMAND_PIDS[command_pos] = reg_msg->sender_pid;
		COMMAND_CHARS[command_pos] = *((char *)reg_msg->msg_data);
		command_pos++;
	}
	
	// store input char in buffer, or process buffer if Enter is pressed
	if (input != 13) {
		KEYBOARD_INPUT_BUFFER[curr_key_buf_pos++] = input;
		input_display[0] = input;
		input_display[1] = '\0';
		key_msg->msg_data = input_display;
		k_send_message(CRT_PID, key_msg);
//		k_crt_i_process();
		return;
	}
	else {
		KEYBOARD_INPUT_BUFFER[curr_key_buf_pos++] = '\0';
		input_display[0] = '\n';
		input_display[1] = '\r';
		input_display[2] = '\0';
		key_msg->msg_data = input_display;
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
	key_msg->msg_data = (void *)b;
	
	// handle %C ourselves
	if (command[0] == 'C') {
		b++;
		// get first parameter: pid
		i = 0;
		while (*b != ' ' && *b != '\0') {
			pid_str[i] = *b;
			b++;
			i++;
		}
		pid_str[i] = '\0';
		pid = str2int(pid_str);
		
		// get second parameter: new priority
		i = 0;
		while (*b != ' ' && *b != '\0') {
			priority_str[i] = *b;
			b++;
			i++;
		}
		priority_str[i] = '\0';
		priority = str2int(priority_str);
		
		// set process priority
		i = k_set_process_priority(pid, priority);
		if (i == -1) {
			k_send_message(CRT_PID, error_msg);
		}
		return;
	}
	
	// handle command types
	for (i = 0; i < command_pos; i++) {
		if (command[0] == COMMAND_CHARS[i]) {
			k_send_message(COMMAND_PIDS[i], key_msg);
		}
	}
	/*
	if (command[0] == 'W') {
		if (command[1] == 'R') {
			// wall clock reset
			WALL_CLOCK_START_TIMER = get_current_time();
			ts_to_hms(0, CURR_TIME_BUFFER);
			WALL_CLOCK_RUNNING = 1;
			
			msg =(MSG*) k_request_memory_block();
			msg->msg_data = saved_process;
			k_send_message(WALL_CLOCK_PID, msg);
			
      iState = k_get_interrupt_state();
      k_set_interrupt_state(iState | 1);
			
			// check if at start the timer started before processes started being schedule, if so no context switch
			if(!(saved_process->pid == 0 && saved_process->state == NEW))
				k_context_switch(wall_clock_pcb);
			
      k_set_interrupt_state(iState);
		}
		else if (command[1] == 'S') {
			// wall clock set
			WALL_CLOCK_START_TIMER = get_current_time();
			ts_to_hms(hms_to_ts((char *)b), CURR_TIME_BUFFER);
			WALL_CLOCK_RUNNING = 1;
			
			msg =(MSG*) k_request_memory_block();
			msg->msg_data = saved_process;
			k_send_message(WALL_CLOCK_PID, msg);
			
      iState = k_get_interrupt_state();
      k_set_interrupt_state(iState | 1);
			
			// check if at start the timer started before processes started being schedule, if so no context switch
			if(!(saved_process->pid == 0 && saved_process->state == NEW))
				k_context_switch(wall_clock_pcb);
			
      k_set_interrupt_state(iState);
		}
		else if (command[1] == 'T') {
	
			// wall clock terminate
			WALL_CLOCK_RUNNING = 0;
			
			// check if at start the timer started before processes started being schedule, if so no context switch
			if(!(saved_process->pid == 0 && saved_process->state == NEW))
				k_context_switch(saved_process);
		}
	}
	*/
}
