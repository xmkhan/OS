#include "keyboard.h"
#include "memory.h"
#include "timer.h"
#include "message.h"
#include "crt_display.h"

int KEYBOARD_PID = 12;
int WALL_CLOCK_START_TIMER = 0;
int WALL_CLOCK_CURRENT_TIMER = 0;
int WALL_CLOCK_RUNNING = 0;

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
	hours = ts % 3600;
	ts /= 3600;
	mins = ts % 60;
	ts /= 60;
	secs = ts;
	
	buffer[0] = hours / 10;
	buffer[1] = hours % 10;
	buffer[2] = ':';
	buffer[3] = mins / 10;
	buffer[4] = mins % 10;
	buffer[5] = ':';
	buffer[6] = secs / 10;
	buffer[7] = secs % 10;
}

void keyboard_proc(char *input)
{
	volatile char command[2];
	volatile int i = 0;
	
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
			WALL_CLOCK_CURRENT_TIMER = WALL_CLOCK_START_TIMER;
			WALL_CLOCK_RUNNING = 1;
		}
		else if (command[1] == 'S') {
			// wall clock set
			WALL_CLOCK_START_TIMER = get_current_time();
			WALL_CLOCK_CURRENT_TIMER = WALL_CLOCK_START_TIMER;
			WALL_CLOCK_RUNNING = 1;
		}
		else if (command[1] == 'T') {
			// wall clock terminate
			WALL_CLOCK_RUNNING = 0;
		}
	}
}
