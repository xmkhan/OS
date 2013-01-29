#include "process.h"

#define NUM_PRIORITIES 256

#ifndef NULL
#define NULL (void *)0
#endif

static Process *p_pq[NUM_PRIORITIES];

static Process *current_process = NULL;

static unsigned int pid = 0;

int k_release_processor(void) {
	return 0;
}


int insert_pq(Process* p) {
  
    return 0;
}

int remove_pq(Process* p) {
return 0;
}

int scheduler(void) {
return 0;
}



void process_init(void) {
//  unsigned int loc = (unsigned int) 
  
}
