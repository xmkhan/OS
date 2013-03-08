#include "keyboard.h"
#include "memory.h"
#include "message.h"
#include "crt_display.h"

int KEYBOARD_PID = 11;
int WALL_CLOCK_START_TIMER = 0;
int WALL_CLOCK_CURRENT_TIMER = 0;
int WALL_CLOCK_RUNNING = 0;

// temporary, will be replaced later by real code
int get_timer_count()
{
	return 0;
}

int hms_to_ts(char *hms)
{
	int hours = (int)((hms[0]-48) * 10 + hms[1]-48);
	int mins = (int)((hms[3]-48) * 10 + hms[4]-48);
	int secs = (int)((hms[6]-48) * 10 + hms[7]-48);
	return (hours*3600) + (mins*60) + (secs);
}

void keyboard_proc(char *input)
{
	//volatile MSG *m = (MSG *)k_request_memory_block();
	volatile char command[2];
	volatile int i = 0;
	
	volatile char *b = input;
	//m->destination_pid = 12;
	//m->msg_type = 1;
	//m->msg_data = input;
	
  if (!b) {
		return;
	}

	// display keyboard input as well
	//crt_proc((void *)b);
	
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
			WALL_CLOCK_START_TIMER = get_timer_count();
			WALL_CLOCK_CURRENT_TIMER = get_timer_count();
			WALL_CLOCK_RUNNING = 1;
		}
		else if (command[1] == 'S') {
			// wall clock set
			WALL_CLOCK_START_TIMER = get_timer_count();
			//WALL_CLOCK_CURRENT_TIMER = get_timer_count() + hms_to_ts(b);
			WALL_CLOCK_RUNNING = 1;
		}
		else if (command[1] == 'T') {
			// wall clock terminate
			WALL_CLOCK_RUNNING = 0;
		}
	}
}
