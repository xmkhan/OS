#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_PID 12

typedef struct PCB PCB;

extern PCB *keyboard_pcb;

void keyboard_init(void);

// does not handle hotkeys, only normal keyboard input
void keyboard_proc(char, PCB *);

#endif
