#ifndef SET_PROCESS_PCB_H
#define SET_PROCESS_PCB_H

#define SET_PROCESS_PCB_PID 15

typedef struct PCB PCB;

extern PCB *set_process_pcb_pcb;

void set_process_pcb_init(void);

void set_process_pcb_proc(void);

#endif
