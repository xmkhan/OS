#ifndef USR_PROC
#define USR_PROC

void __initialize_processes(void);

#define NUM_PROCESSES 6

extern PCB *pcb_list[];
extern Process process_list[];

void null_process(void);
void proc1(void);
void proc2(void);
void proc3(void);
void proc4(void);
void proc5(void);

#endif
