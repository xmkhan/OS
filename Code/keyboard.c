#include "keyboard.h"
#include "crt_display.h"

void keyboard_proc(MSG *m)
{
  volatile char *b = m->msg_data;
	volatile char command[2];
	volatile int i = 0;
	
  if (!b) {
		return;
	}

	// display keyboard input as well
	crt_proc(m);
	
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
	if (command[0] == 'W' && command[1] == 'S') {
		// wall clock
	}
}
