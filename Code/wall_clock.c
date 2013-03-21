#include "wall_clock.h"

int WALL_CLOCK_START_TIMER = 0;
char CURR_TIME_BUFFER[11];

Process wall_clock_process;
PCB *wall_clock_pcb;

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
