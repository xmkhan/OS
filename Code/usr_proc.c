#include "process.h"
#include "memory.h"
#include "message.h"
#include "usr_proc.h"
#include "crt_display.h"
#include "pq.h"

#define INITIAL_xPSR 0x01000000

#define KEYBOARD_ENABLED

// Tri-state variable -1 = fail, 0 = not tested, 1 = pass
#define NUM_TESTS 5
volatile int TEST1 = 0;
volatile int TEST2 = 0;
volatile int TEST3 = 0;
volatile int TEST4 = 0;
volatile int TEST5 = 0;
// process list
PCB *pcb_list[NUM_PROCESSES];
Process process_list[NUM_PROCESSES];

typedef void (*process_ptr)(void);

// set up each process (including null) and their stack frames
void __initialize_processes(void) {
  uint32_t *sp = (void *)0;
  volatile unsigned int i = 0, j = 0, k = 0;

  process_ptr process_t[] = {null_process, proc1, proc2, proc3, proc4, proc5, proc6};
  int priority_t[] = {4, 2, 2, 2, 2, 2, 2};

  for(k = 0; k < NUM_PROCESSES; k++) {
    pcb_list[k] = k_request_memory_block();
    process_list[k].stack = k_request_memory_block();
  }

  for (; i < NUM_PROCESSES; i++) {
    pcb_list[i]->pid = i;
    pcb_list[i]->priority = priority_t[i];
    pcb_list[i]->type = USER;
    pcb_list[i]->state = NEW;
    pcb_list[i]->status = NONE;
    pcb_list[i]->head = (void *)0;
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
  }
}

volatile int msg1_status =0;
volatile int msg2_status=0;
// process 1: Testing for message sending
void proc1(void)
{
  while(1)
  {
		volatile int ret_val = 10;
		#ifndef KEYBOARD_ENABLED
    volatile int status = 10;
    volatile int send_status = 10;
    int x1 = 10, x2 = 20;
    volatile MSG *msg = (void *)0;
    volatile MSG *msg2 = (void *)0;
	  // Output test results
		crt_print("G013_test: START\n\rG013_test: total 7 tests\n\r");

    TEST1 = 0;
    if(msg1_status ==0) {
      msg = (volatile MSG *) request_memory_block();
      msg->msg_data = (void *) &x1;
      msg->msg_type = 1;
      send_status = send_message(2, (MSG *)msg);
      msg1_status = 1;
    }
    if (msg2_status ==0){
      msg2 = (volatile MSG *) request_memory_block();
      msg2->msg_data = (void *) &x2;
      msg2->msg_type = 1;
      send_status = send_status  | send_message(2, (MSG *)msg2);
      msg2_status = 1;
    }

    if (send_status == 0 || (msg1_status == 1 && msg2_status == 1)) {
      crt_print("G013_test: test 1 OK\n\r");
      TEST1 = 1;
    } else {
      crt_print("G013_test: test 1 FAIL\n\r");
      TEST1 = -1;
    }
		#endif
    ret_val = release_processor();
  }
}

// process 2: Testing for message receiving
void proc2(void)
{
  while(1)
  {
    volatile int ret_val = 10;
    #ifndef KEYBOARD_ENABLED
    volatile MSG *msg = (void *)0;
    volatile MSG *msg2 = (void *)0;
    int sender_pid = -1;
    int pass = 0;
    int release_status = 0;
    TEST2 = 0;
    msg = receive_message(&sender_pid);
    msg2 = receive_message(&sender_pid);

    if (*((int *)msg->msg_data) == 10 && *((int *)msg2->msg_data) == 20) {
      pass++;
      release_status = release_memory_block((void *)msg);
      msg1_status = 0;
      release_status = release_status | release_memory_block((void *)msg2);
      msg2_status = 0;
    }

    if (release_status == 0) {
      pass++;
    }
    if (pass == 2) {
      crt_print("G013_test: test 2 OK\n\r");
      TEST2 = 1;
    } else {
      crt_print("G013_test: test 2 FAIL\n\r");
      TEST2 = -1;
    }
    #endif
    ret_val = release_processor();
  }
}

// process 3: Testing for multiple send/multiple receive
void proc3(void)
{
  while(1) {
    volatile int ret_val = 10;
    #ifndef KEYBOARD_ENABLED
    int sender_pid = -1, pass = 0, x3 = 30;
    volatile MSG *msg = (void *)0;
    volatile MSG *received_msg = (void *)0;
    TEST3 = 0;
    msg = (volatile MSG *)request_memory_block();
    msg->msg_data = (void *) &x3;
    msg->msg_type = 1;
    delayed_send(3, (MSG *) msg, 10);

    received_msg = receive_message(&sender_pid);
    if (*((int *)received_msg->msg_data) == 30) {
      pass++;
    }

    if (pass == 1) {
     crt_print("G013_test: test 3 OK\n\r");
     TEST3 = 1;
   } else {
     crt_print("G013_test: test 3 FAIL\n\r");
     TEST3 = -1;
   }
    release_memory_block((void *)msg);
   #endif
   ret_val = release_processor();
 }
}

// process 4: test process for getting an setting process priorities
void proc4(void)
{
  while (1) {
    volatile int ret_val = 10;
    volatile int pass = 0;
    volatile int i = 0;
    TEST4 = 0;
    #ifndef KEYBOARD_ENABLED
    i = get_process_priority(4);
    if ( i == 2) {
      pass++;
    }
    i = set_process_priority(4, 3);
    i = get_process_priority(4);
    if (i == 3) {
      pass++;
    }
    i = set_process_priority(4,2);
    i = get_process_priority(4);
    if (i == 2) {
      pass++;
    }
    if (pass == 3) {
      crt_print("G013_test: test 4 OK\n\r");
      TEST4 = 1;
    } else {
      crt_print("G013_test: test 4 FAIL\n\r");
      TEST4 = -1;
    }
    #endif
  ret_val = release_processor();
  }
}

// process 5: test registers/general variable allocation
void proc5(void)
{
  while (1) {
  volatile int ret_val = 10;
  volatile int pass = 0;

  #ifndef KEYBOARD_ENABLED
  volatile int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6, g = 7, h = 8, i = 9, j = 10,
  k = 11, l = 12, m = 13, n = 14, o = 15, p = 16, q = 17, r = 18, s = 19, t = 20,
  u = 21, v = 22, w = 23, x = 24, y = 25, z = 26;
  a +=1; b += 1; c +=1; d +=1; e +=1; f +=1;g+=1;h+=1;i+=1;j+=1;k+=1;l+=1;m+=1;n+=1;o+=1;
  p+=1;q+=1;r+=1;s+=1;t+=1;u+=1;v+=1;w+=1;x+=1;y+=1;z+=1;
  TEST5 = 0;
  if (a == 2 && b == 3 && c == 4 && d == 5 && e == 6 && f == 7 && g == 8 && h == 9
				&& i == 10 && j == 11 && k == 12 && l == 13 && m == 14 && n == 15 && o == 16 && p == 17
				&& q == 18 && r == 19 && s == 20 && t == 21 && u == 22 && v == 23 && w == 24 && x == 25
				&& y == 26 && z == 27) {
          pass++;
  }
  if (pass == 1) {
    crt_print("G013_test: test 5 OK\n\r");
    TEST5 = 1;
  } else {
    crt_print("G013_test: test 5 FAIL\n\r");
    TEST5 = -1;
  }
  #endif
  ret_val = release_processor();
  }
}

// process 6: end of tests, print results
void proc6(void)
{

  while (1) {
    volatile int ret_val = 20;
		#ifndef KEYBOARD_ENABLED
    volatile int TEST_NUM_PASSED = 0;
		const char *PASS_STRING = "G013_test: \a/5 OK\n\rG013_test: \a/5 FAIL\n\r";
		char buffer[50];
		int count = 0;
		int count_copy = 0;
		
   
    if (TEST1 != -1) TEST_NUM_PASSED++;
    if (TEST2 != -1) TEST_NUM_PASSED++;
    if (TEST3 != -1) TEST_NUM_PASSED++;
    if (TEST4 != -1) TEST_NUM_PASSED++;
    if (TEST5 != -1) TEST_NUM_PASSED++;
		
		while(PASS_STRING[count] != '\0') {
			if (PASS_STRING[count] == '\a') {
				int integer_counter = 0;
				char integer_buffer[20];
				int2str(TEST_NUM_PASSED, integer_buffer);
				TEST_NUM_PASSED = NUM_TESTS - TEST_NUM_PASSED;
				while (integer_buffer[integer_counter] != '\0') {
					buffer[count_copy++] = integer_buffer[integer_counter];
					integer_counter++;
				}
			} else {
				buffer[count_copy++] = PASS_STRING[count];
			}
			count++;
		}
		buffer[count] = '\0';
		crt_print(buffer);

    #endif
    ret_val = release_processor();
  }
}

void procA(void) {
  volatile MSG *msg_envelope  = (void *)0;
  volatile MSG *msg           = (void *)0;
  volatile int release_status = 0;
  volatile int send_status    = 10;
  int sender_pid              = -1;
  volatile int ret_val        = 20;
  int num                     = 0;
  volatile char* msg_body     = (void *)0;

  msg = (volatile MSG *) request_memory_block();
  //register with command decoder as handler of %Z commands
  while(1) {
    msg = receive_message(&sender_pid);
    msg_body = (char*)msg->msg_data;
    if(msg_body[0] == '%'
      && msg_body[1] == 'Z') {
        release_status = release_memory_block((void *)msg);
        break;
    }
    else {
      release_status = release_memory_block((void *)msg);
    }
  }

  while(1) {
    msg_envelope = (volatile MSG *) request_memory_block();
    msg_envelope->msg_type = 3; // count_report
    msg_envelope->msg_data = &num;
    send_status = send_message(8, (void*)msg_envelope);
    num += 1;
    ret_val = release_processor();
  }
}

void procB(void) {
  volatile MSG* msg = (void*) 0;
  int sender_pid    = -1;
  while(1) {
    msg = receive_message(&sender_pid);
    send_message(9, (void*)msg);
  }
}

void procC(void) {
  volatile MSG* head          = (void *) 0;
  volatile MSG* msg           = (void *) 0;
  volatile MSG* q             = (void *) 0;
  volatile int release_status = 0;
  volatile int sent_status    = 10;
  volatile int ret_val        = 20;
  int sender_pid              = -1;
  int* msg_int                = (void*) 0;

  while(1) {
    if(head == (void*) 0) {
      msg = receive_message(&sender_pid);
    }
    else {
      msg = dequeue_q(&head, MSG_T);
    }
    if (msg->msg_type == 3) { // count_report
      msg_int = (int*) msg->msg_data;
      if(*msg_int % 20 == 0) {
        //send message to crt
        send_message(CRT_PID, (void*)msg);

        //send delayed messaged
        q = (volatile MSG *) request_memory_block();
        q->msg_type = 4;
        delayed_send(9, (MSG *) q, 10 * 1000);

        //add messages to queue while waiting for delayed message
        while(1) {
          msg = receive_message(&sender_pid);
          if(msg->msg_type == 4) {
            break;
          }
          else {
            enqueue_q(&head, (void*)msg, MSG_T);
          }
        }
      }
    }
    release_status = release_memory_block((void *)msg);
    ret_val = release_processor();
  }
}
