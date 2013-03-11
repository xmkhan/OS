#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_PID 12
#define WALL_CLOCK_PID 13
extern int WALL_CLOCK_START_TIMER;
extern int WALL_CLOCK_CURRENT_TIMER;
extern int WALL_CLOCK_RUNNING;

typedef struct PCB PCB;

extern PCB *keyboard_pcb;

void keyboard_init(void);
void wall_clock_init(void);

// does not handle hotkeys, only normal keyboard input
void keyboard_proc(char *);

// process for displaying wall clock
void wall_clock(void);

#endif
