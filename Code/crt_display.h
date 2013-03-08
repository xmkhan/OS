#ifndef CRT_DISPLAY_H
#define CRT_DISPLAY_H

// PID for crt
#define CRT_PID 11

typedef struct PCB PCB;
extern PCB* crt_pcb;

// initialize the crt
void crt_init(void);

// convert an integer to char*
void int_to_char_star(int input, volatile char* buffer);

// output to display
void crt_proc(char* m);

// output an int to display
void crt_output_int(int input);

// interrupt routine
void crt_interrupt(void);

// handle hot-key key press
void hot_key_handler(void);

#endif

