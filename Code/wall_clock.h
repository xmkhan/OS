#ifndef WALL_CLOCK_H
#define WALL_CLOCK_H

#define WALL_CLOCK_PID 13

extern int WALL_CLOCK_START_TIMER;
extern int WALL_CLOCK_CURRENT_TIMER;
extern int WALL_CLOCK_RUNNING;

typedef struct PCB PCB;

extern PCB *wall_clock_pcb;

void wall_clock_init(void);

// process for displaying wall clock
void wall_clock(void);

#endif
