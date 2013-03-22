#include "set_process_pcb.h"
#include "memory.h"
#include "crt_display.h"
#include "message.h"
#include "keyboard.h"

Process set_process_pcb_process;
PCB *set_process_pcb_pcb;

void set_process_pcb_init(void) {

  uint32_t *sp = (void *)0;
  int j = 0;
  MSG *init_set_process_msg = (void *)0;

  // initialize keyboard display process
  set_process_pcb_pcb = k_request_memory_block();
  set_process_pcb_process.stack = k_request_memory_block();

  set_process_pcb_pcb->pid = SET_PROCESS_PCB_PID;
  set_process_pcb_pcb->priority = 1;
  set_process_pcb_pcb->type = SYSTEM;
  set_process_pcb_pcb->state = NEW;
  set_process_pcb_pcb->head = (void *) 0;
  set_process_pcb_pcb->next = (void *) 0;
  set_process_pcb_process.pcb = set_process_pcb_pcb;
  set_process_pcb_process.start_loc = (uint32_t)set_process_pcb_proc;

  sp = (uint32_t *)((uint32_t)set_process_pcb_process.stack + MEMORY_BLOCK_SIZE);

  /* 8 bytes alignement adjustment to exception stack frame */
  if (!(((uint32_t)sp) & 0x04)) {
      --sp;
  }

  *(--sp)  = 0x01000000;              /* user process initial xPSR */
  *(--sp)  = set_process_pcb_process.start_loc ;  /* PC contains the entry point of the process */

  for (j=0; j < 6; j++) {             /* R0-R3, R12 are cleared with 0 */
    *(--sp) = 0x0;
  }
  set_process_pcb_process.pcb->mp_sp = (uint32_t *)sp;

  insert_process_pq(set_process_pcb_process.pcb);

  current_process = set_process_pcb_pcb;

  init_set_process_msg = (MSG *)k_request_memory_block();
  init_set_process_msg->msg_type = 4;
  init_set_process_msg->msg_data = "C";
  k_send_message(KEYBOARD_PID, init_set_process_msg);
}

int str2int(char *s) {
	char *t = s;
	int total = 0;
	int multiplier = 1;
	
	while (*t != '\0') {
		t++;
	}
	t--;
	
	while (t != s) {
		total += ((*t) - 48) * multiplier;
		multiplier *= 10;
		t--;
	}
	total += ((*t) - 48) * multiplier;
	return total;
}

void set_process_pcb_proc(void) {
	while (1) {
		char pid_str[4];
		int pid;
		char priority_str[4];
		int priority;
		
		int sender_pid = 0;
		MSG *command = receive_message(&sender_pid);
		MSG *error_msg = (void *)0;
		
		char *b = (char *)command->msg_data;
		int i = 0;
		
		if (*b == 'C') {
			b += 2;
			
			// get first parameter: pid
			i = 0;
			while (*b != ' ' && *b != '\0') {
				pid_str[i] = *b;
				b++;
				i++;
			}
			if (*b == ' ') {
				b++;
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
			if (*b == ' ') {
				b++;
			}
			
			priority_str[i] = '\0';
			priority = str2int(priority_str);
			
			// set process priority
			i = set_process_priority(pid, priority);
			if (i == -1) {
				error_msg = (MSG *)request_memory_block();
				error_msg->msg_type = 1;
				error_msg->msg_data = "Invalid command.\n\r";
				send_message(CRT_PID, error_msg);
			}
		}
		release_processor();
	}
}

