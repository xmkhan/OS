#ifndef USR_PROC
#define USR_PROC

void __initialize_processes(void);

#define NUM_PROCESSES 3

extern PCB *pcb_list[];
extern Process process_list[];

void null_process(void);
void proc1(void);
void proc2(void);

#endif
