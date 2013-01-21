#ifndef PROCESS_H
#define PROCESS_H

#ifndef __SVC_0
#define __SVC_0  __svc_indirect(0)
#endif


typedef struct Process {
	uint8_t priority;
  
} Process;


Process NullProcess;

extern int k_release_processor(void);
#define release_processor() _release_processor((unsigned int)k_release_processor)
extern int _release_processor(unsigned int p_func) __SVC_0;

extern int k_set_process_priority(void);
#define set_process_priority() _set_process_priority((unsigned int)k_set_process_priority)
extern int _set_process_priority(unsigned int p_func) __SVC_0;

extern int k_get_process_priority(void);
#define get_process_priority() _get_process_priority((unsigned int)k_get_process_priority)
extern int _get_process_priority(unsigned int p_func) __SVC_0;

#endif
