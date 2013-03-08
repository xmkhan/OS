#ifndef USR_PROC
#define USR_PROC

typedef struct PCB PCB;
typedef struct Process Process;

void __initialize_processes(void);

#define NUM_PROCESSES 7

typedef struct PCB PCB;
typedef struct Process Process;

extern PCB *pcb_list[];
extern Process process_list[];

void null_process(void);
void proc1(void);
void proc2(void);
void proc3(void);
void proc4(void);
void proc5(void);
void proc6(void);

#endif
