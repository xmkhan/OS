#include "process.h"
#include "memory.h"
#include "usr_proc.h"
#include "crt_display.h"

#ifdef DEBUG
#include <stdio.h>
#endif  /* DEBUG */

#define INITIAL_xPSR 0x01000000

// test state variables
int TEST1 = 0;
int TEST2 = 0;
int TEST3 = 0;
int TEST4 = 0;
int TEST5 = 0;
int TEST6 = 0;
int TEST7 = 0;
int TESTEND = 0;
int NUM_TESTS_PASSED = 0;
int NUM_TESTS_FAILED = 0;

// process list
PCB *pcb_list[NUM_PROCESSES];
Process process_list[NUM_PROCESSES];

typedef void (*process_ptr)(void);

// set up each process (including null) and their stack frames
void __initialize_processes(void) {
  uint32_t *sp = (void *)0;
  volatile unsigned int i = 0, j = 0, k = 0;  
  
  process_ptr process_t[] = {null_process, proc1, proc2, proc3, proc4, proc5, proc6};
  int priority_t[] = {4, 0, 1, 1, 1, 1, 1};

  for(k = 0; k < NUM_PROCESSES; k++) {
    pcb_list[k] = k_request_memory_block();
    process_list[k].stack = k_request_memory_block();
  }

  for (; i < NUM_PROCESSES; i++) {
    pcb_list[i]->pid = i;
    pcb_list[i]->priority = priority_t[i];
    pcb_list[i]->type = USER;
    pcb_list[i]->state = NEW;
    process_list[i].pcb = (PCB *)pcb_list[i];
    process_list[i].start_loc = (uint32_t)process_t[i];
    process_list[i].pcb->next = (void *)0;
    
    sp = (uint32_t *)((uint32_t)process_list[i].stack + MEMORY_BLOCK_SIZE);
    
    /* 8 bytes alignement adjustment to exception stack frame */
    if (!(((uint32_t)sp) & 0x04)) {
        --sp;
    }
    
    *(--sp)  = INITIAL_xPSR;      /* user process initial xPSR */ 
    *(--sp)  = (uint32_t)process_t[i];  /* PC contains the entry point of the process */

    for (j=0; j < 6; j++) { /* R0-R3, R12 are cleared with 0 */
      *(--sp) = 0x0;
    }
    
    process_list[i].pcb->mp_sp = (uint32_t *)sp;
  }
}

// null process: simply release processor
void null_process(void) {
  while(1) {
    volatile int ret_val = release_processor();
    crt_proc("\n\rnull_proc: ret_val=");
    crt_output_int(ret_val); 
  }
}

// process 1: request two memory blocks and test assignment/position difference,
// then request all remaining memory until process gets blocked
void proc1(void)
{
  volatile int i = 0;
  volatile int ret_val = 10;
  volatile void *a = request_memory_block();
	volatile int *int1 = (int *)request_memory_block();
	volatile int *int2 = (int *)request_memory_block();
	
	*int1 = 42;
	if (*int1 == 42) {
		NUM_TESTS_PASSED++;
		TEST1 = 1;
	}
	else {
		NUM_TESTS_FAILED++;
		TEST1 = 3;
	}
	
	if ((uint32_t)int2 - (uint32_t)int1 == (uint32_t)MEMORY_BLOCK_SIZE + (uint32_t)sizeof(MemNode)) {
		NUM_TESTS_PASSED++;
		TEST2 = 1;
	}
	else {
		NUM_TESTS_FAILED++;
		TEST2 = 3;
	}
	hot_key_handler();
  while (1) {
		break;
		a = request_memory_block();
  }
	if (!TEST3) {
		TEST3 = 1;
		NUM_TESTS_FAILED++;
    crt_proc("\n\rG013_test: test 3 FAIL");
	}
	while (1) {
		ret_val = release_processor();
	}
}

// process 2: called after proc1 gets blocked, test proc1 results
void proc2(void)
{
  volatile int i = 0;
  volatile int ret_val = 20;
	
	// values get stored in memory correctly?
	if (TEST1 == 1) {
		TEST1 = 2;
    crt_proc("\n\rG013_test: test 1 OK");
	}
	else if (TEST1 == 3) {
		TEST1 = 4;
    crt_proc("\n\rG013_test: test 1 FAIL");
	}
	
	// memory offsets correct?
	if (TEST2 == 1) {
		TEST2 = 2;
    crt_proc("\n\rG013_test: test 2 OK");
	}
	else if (TEST2 == 3) {
		TEST2 = 4;
    crt_proc("\n\rG013_test: test 2 FAIL");
	}
	
	// blocking from out of memory worked?
	if (!TEST3) {
		TEST3 = 1;
		NUM_TESTS_PASSED++;
    crt_proc("\n\rG013_test: test 3 OK");
	}
	i = set_process_priority(2, 4);
	if (i == -1) {
		if (!TEST4) {
			TEST4 = 1;
			NUM_TESTS_PASSED++;
      crt_proc("\n\rG013_test: test 4 OK");
		}
	} else {
		if (!TEST4) {
			TEST4 = 1;
			NUM_TESTS_FAILED++;
      crt_proc("\n\rG013_test: test 4 FAIL");
		}
	}
	while (1) {
		ret_val = release_processor();
	}
}

// process 3: test get process priority
void proc3(void)
{
  volatile int i = 0;
  volatile int ret_val = 20;
  i = get_process_priority(3);
	if (i == 1) {
		if (!TEST5) {
			TEST5 = 1;
			NUM_TESTS_PASSED++;
      crt_proc("\n\rG013_test: test 5 OK");
		}
	}
	else {
		if (!TEST5) {
			TEST5 = 1;
			NUM_TESTS_FAILED++;
      crt_proc("\n\rG013_test: test 5 FAIL");
		}
	}
	while (1) {
		ret_val = release_processor();
	}
}

// process 4: set process priority, then block by requesting memory (already gone from proc1)
void proc4(void)
{
	static volatile int counter = 0;
  volatile int i = 0;
  volatile int ret_val = 20;
	void *a;
	set_process_priority(4, 0);
	a = request_memory_block();
	while (1) {
		ret_val = release_processor();
	}
}

// process 5: test registers/general variable allocation and results of proc4
void proc5(void)
{
  volatile int ret_val = 20;
	int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6, g = 7, h = 8, i = 9, j = 10,
		k = 11, l = 12, m = 13, n = 14, o = 15, p = 16, q = 17, r = 18, s = 19, t = 20,
		u = 21, v = 22, w = 23, x = 24, y = 25, z = 26;
		if (!TEST6) {
			TEST6 = 1;
			NUM_TESTS_PASSED++;
      crt_proc("\n\rG013_test: test 6 OK");
		}
  while (1) {
    ret_val = release_processor();
		if (!TEST7) {
			TEST7 = 1;
			if (a == 1 && b == 2 && c == 3 && d == 4 && e == 5 && f == 6 && g == 7 && h == 8
				&& i == 9 && j == 10 && k == 11 && l == 12 && m == 13 && n == 14 && o == 15 && p == 16
				&& q == 17 && r == 18 && s == 19 && t == 20 && u == 21 && v == 22 && w == 23 && x == 24
				&& y == 25 && z == 26) {
					NUM_TESTS_PASSED++;
          crt_proc("\n\rG013_test: test 7 OK");
				}
				else {
					NUM_TESTS_FAILED++;
          crt_proc("\n\rG013_test: test 7 FAIL");
				}
			}
  }
}

// process 6: end of tests, print results
void proc6(void)
{
	volatile int ret_val = 20;
  while (1) {
    ret_val = release_processor();
			if (!TESTEND) {
			TESTEND = 1;
      crt_proc("\n\rG013_test: ");
      crt_output_int(NUM_TESTS_PASSED); 
      crt_proc("/7 tests OK");
      crt_proc("\n\rG013_test: ");
      crt_output_int(NUM_TESTS_FAILED); 
      crt_proc("/7 tests FAIL");
      crt_proc("\n\rG013_test: END");
		}
  }
}
