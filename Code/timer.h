#ifndef _TIMER_H_
#define _TIMER_H_

// PID for timer
#define TIMER_PID 10

typedef struct PCB PCB;
extern PCB* timer_pcb;

// initialize the timer
uint32_t timer_init ( uint8_t n_timer );

// process which gets executed when timer interrupt occurs
void timeout_i_process(void);

#endif /* ! _TIMER_H_ */
