#include "process.h"
#include "memory.h"
#include "uart_polling.h"
#include "usr_proc.h"

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif  /* DEBUG */

#define INITIAL_xPSR 0x01000000

int TEST1 = 0;
int TEST2 = 0;
int TEST3 = 0;
int TEST4 = 0;
int TEST5 = 0;
int TESTEND = 0;
int NUM_TESTS_PASSED = 0;
int NUM_TESTS_FAILED = 0;

PCB *pcb_list[NUM_PROCESSES];
Process process_list[NUM_PROCESSES];

typedef void (*process_ptr)(void);

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
    pcb_list[i]->state = NEW;
    process_list[i].pcb = (PCB *)pcb_list[i];
    process_list[i].start_loc = (uint32_t)process_t[i];
    process_list[i].pcb->next = (void *)0;
    
    sp = (uint32_t *)((uint32_t)process_list[i].stack + MEMORY_BLOCK_SIZE_HEX);
    
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

void null_process(void) {
  while(1) {
    volatile int ret_val = release_processor();
#ifdef DEBUG
      printf("\n\rnull_proc: ret_val=%d. ", ret_val);
#endif
  }
}

void proc1(void)
{
  volatile int i = 0;
  volatile int ret_val = 10;
  void *a = request_memory_block();
  while (1) {
		a = request_memory_block();
  }
	if (!TEST1) {
		TEST1 = 1;
		NUM_TESTS_FAILED++;
#ifdef DEBUG
		printf("\n\rG013_test: test 1 FAIL");
#else
		uart0_put_string("\n\rG013_test: test 1 FAIL");
#endif
	}
	while (1) {
		ret_val = release_processor();
	}
}

void proc2(void)
{
  volatile int i = 0;
  volatile int ret_val = 20;
	if (!TEST1) {
		TEST1 = 1;
		NUM_TESTS_PASSED++;
#ifdef DEBUG
		printf("\n\rG013_test: test 1 OK");
#else
		uart0_put_string("\n\rG013_test: test 1 OK");
#endif
	}
	i = set_process_priority(2, 4);
	if (i == -1) {
		if (!TEST2) {
			TEST2 = 1;
			NUM_TESTS_PASSED++;
#ifdef DEBUG
			printf("\n\rG013_test: test 2 OK");
#else
			uart0_put_string("\n\rG013_test: test 2 OK");
#endif
		}
	} else {
		if (!TEST2) {
			TEST2 = 1;
			NUM_TESTS_FAILED++;
#ifdef DEBUG
			printf("\n\rG013_test: test 2 FAIL");
#else
			uart0_put_string("\n\rG013_test: test 2 FAIL");
#endif
		}
	}
	while (1) {
		ret_val = release_processor();
	}
}

void proc3(void)
{
  volatile int i = 0;
  volatile int ret_val = 20;
  i = get_process_priority(3);
	if (i == 1) {
		if (!TEST3) {
			TEST3 = 1;
			NUM_TESTS_PASSED++;
#ifdef DEBUG
			printf("\n\rG013_test: test 3 OK");
#else
			uart0_put_string("\n\rG013_test: test 3 OK");
#endif
		}
	}
	else {
		if (!TEST3) {
			TEST3 = 1;
			NUM_TESTS_FAILED++;
#ifdef DEBUG
			printf("\n\rG013_test: test 3 FAIL");
#else
			uart0_put_string("\n\rG013_test: test 3 FAIL");
#endif
		}
	}
	while (1) {
		ret_val = release_processor();
	}
}

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

void proc5(void)
{
  volatile int ret_val = 20;
	int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6, g = 7, h = 8, i = 9, j = 10,
		k = 11, l = 12, m = 13, n = 14, o = 15, p = 16, q = 17, r = 18, s = 19, t = 20,
		u = 21, v = 22, w = 23, x = 24, y = 25, z = 26;
		if (!TEST4) {
			TEST4 = 1;
			NUM_TESTS_PASSED++;
#ifdef DEBUG
			printf("\n\rG013_test: test 4 OK");
#else
			uart0_put_string("\n\rG013_test: test 4 OK");
#endif
		}
  while (1) {
    ret_val = release_processor();
		if (!TEST5) {
			TEST5 = 1;
			if (a == 1 && b == 2 && c == 3 && d == 4 && e == 5 && f == 6 && g == 7 && h == 8
				&& i == 9 && j == 10 && k == 11 && l == 12 && m == 13 && n == 14 && o == 15 && p == 16
				&& q == 17 && r == 18 && s == 19 && t == 20 && u == 21 && v == 22 && w == 23 && x == 24
				&& y == 25 && z == 26) {
					NUM_TESTS_PASSED++;
#ifdef DEBUG
					printf("\n\rG013_test: test 5 OK");
#else
					uart0_put_string("\n\rG013_test: test 5 OK");
#endif
				}
				else {
					NUM_TESTS_FAILED++;
#ifdef DEBUG
					printf("\n\rG013_test: test 5 FAIL");
#else
					uart0_put_string("\n\rG013_test: test 5 FAIL");
#endif
				}
			}
  }
}

void proc6(void)
{
	volatile int ret_val = 20;
  while (1) {
    ret_val = release_processor();
			if (!TESTEND) {
#ifdef DEBUG
			TESTEND = 1;
			printf("\n\rG013_test: %d/5 tests OK", NUM_TESTS_PASSED);
			printf("\n\rG013_test: %d/5 tests FAIL", NUM_TESTS_FAILED);
			printf("\n\rG013_test: END");
#else
			TESTEND = 1;
			uart0_put_string("\n\rG013_test: ");
			uart0_put_char(NUM_TESTS_PASSED+48);
			uart0_put_string("/5 tests OK");
			uart0_put_string("\n\rG013_test: ");
			uart0_put_char(NUM_TESTS_FAILED+48);
			uart0_put_string("/5 tests FAIL");
			uart0_put_string("\n\rG013_test: END");
#endif
		}
  }
}
