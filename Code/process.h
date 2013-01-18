#ifndef PROCESS_H
#define PROCESS_H

#ifndef __SVC_0
#define __SVC_0  __svc_indirect(0)
#endif

typedef struct Process {
	int type;
} Process;

extern int k_release_processor(void);
#define release_processor() _release_processor((unsigned int)k_release_processor)
extern int _release_processor(unsigned int p_func) __SVC_0;

#endif
