#include "keyboard.h"
#include "process.h"
#include "memory.h"
#include "timer.h"
#include "message.h"
#include "crt_display.h"

int WALL_CLOCK_START_TIMER = 0;
char CURR_TIME_BUFFER[11];
int WALL_CLOCK_RUNNING = 0;

Process keyboard_process;
PCB* keyboard_pcb;
Process wall_clock_process;
PCB *wall_clock_pcb;

//static PCB *saved_process = (void *)0;
	
void wall_clock_init(void) {
  uint32_t *sp = (void *)0;
  int j = 0;
  
  // initialize keyboard display process
  wall_clock_pcb = k_request_memory_block();
  wall_clock_process.stack = k_request_memory_block();
  
  wall_clock_pcb->pid = WALL_CLOCK_PID;
  wall_clock_pcb->priority = 99;
  wall_clock_pcb->type = USER;
  wall_clock_pcb->state = NEW;
  wall_clock_pcb->head = (void *) 0;
  wall_clock_pcb->next = (void *) 0;
  wall_clock_process.pcb = wall_clock_pcb;
  wall_clock_process.start_loc = (uint32_t)wall_clock;
     
  sp = (uint32_t *)((uint32_t)wall_clock_process.stack + MEMORY_BLOCK_SIZE);
    
  /* 8 bytes alignement adjustment to exception stack frame */
  if (!(((uint32_t)sp) & 0x04)) {
      --sp;
  }
    
  *(--sp)  = 0x01000000;              /* user process initial xPSR */ 
  *(--sp)  = wall_clock_process.start_loc ;  /* PC contains the entry point of the process */

  for (j=0; j < 6; j++) {             /* R0-R3, R12 are cleared with 0 */
    *(--sp) = 0x0;
  }  
  wall_clock_process.pcb->mp_sp = (uint32_t *)sp;
  
  insert_process_pq(wall_clock_process.pcb);
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

}

int hms_to_ts(char *hms)
{
	int hours = (int)((hms[0]-48) * 10 + hms[1]-48);
	int mins = (int)((hms[3]-48) * 10 + hms[4]-48);
	int secs = (int)((hms[6]-48) * 10 + hms[7]-48);
	return (hours*3600) + (mins*60) + (secs);
}

void ts_to_hms(int ts, char *buffer)
{
	int hours;
	int mins;
	int secs;
	ts %= 86400;
	hours = ts / 3600;
	ts %= 3600;
	mins = ts / 60;
	ts %= 60;
	secs = ts;
	
	buffer[0] = (hours / 10) + 48;
	buffer[1] = (hours % 10) + 48;
	buffer[2] = ':';
	buffer[3] = (mins / 10) + 48;
	buffer[4] = (mins % 10) + 48;
	buffer[5] = ':';
	buffer[6] = (secs / 10) + 48;
	buffer[7] = (secs % 10) + 48;
	buffer[8] = '\r';
	buffer[9] = '\0';
}

void keyboard_proc(char *input, PCB *saved_process)
{
	volatile char command[2];
	volatile int i = 0;

 	int iState;
	
	MSG* msg = (void*) 0;
	volatile char *b = input;
	
  if (!b) {
		return;
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
	
	// skip over space (to get command parameter) or null char
	b++;
	
	// handle command types
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
}

void wall_clock(void)
{
	while(1)
	{
		MSG* msg = (void *)0;
		int counter = WALL_CLOCK_START_TIMER;
		int comparison = WALL_CLOCK_START_TIMER;
		while (WALL_CLOCK_RUNNING) {
			counter = get_current_time();
			if (counter - comparison >= 1000) {
				ts_to_hms(hms_to_ts(CURR_TIME_BUFFER)+1, CURR_TIME_BUFFER);
				crt_print(CURR_TIME_BUFFER);
				comparison = counter;
			}
		}	
		
		msg = get_message(wall_clock_pcb);
	  context_switch(msg->msg_data);		
	}
}
