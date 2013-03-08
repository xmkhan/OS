#ifndef KEYBOARD_H
#define KEYBOARD_H

extern int KEYBOARD_PID;
extern int WALL_CLOCK_START_TIMER;
extern int WALL_CLOCK_CURRENT_TIMER;
extern int WALL_CLOCK_RUNNING;

// does not handle hotkeys, only normal keyboard input
void keyboard_proc(char *);

#endif
