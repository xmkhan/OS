#ifndef USR_PROC
#define USR_PROC

typedef struct PCB PCB;
typedef struct Process Process;

void __initialize_processes(void);

extern int NUM_PROCESSES;

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
void procA(void);
void procB(void);
void procC(void);

#endif
