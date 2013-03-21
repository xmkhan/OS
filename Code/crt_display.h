#ifndef CRT_DISPLAY_H
#define CRT_DISPLAY_H

// PID for crt
#define CRT_PID 11
#define HOTKEY_PID 14

#ifndef __SVC_0
#define __SVC_0  __svc_indirect(0)
#endif

typedef struct PCB PCB;
extern PCB* crt_pcb;
extern PCB* hotkey_pcb;

// initialize the crt
void crt_init(void);

// initialize hotkey process
void hotkey_init(void);

// convert an integer to char*
void int_to_char_star(int input, volatile char* buffer);

// output an int to display
void crt_output_int(int input);

// crt system process
void crt_system_process(void);

// handle hot-key key press
void hot_key_handler(void);

void crt_print(char*);
void k_crt_print(char*);

#endif

