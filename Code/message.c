#include "pq.h"
#include "message.h"
#include "semaphore.h"

semaphore s;
semaphore r;

// PQ storing PCBs which are blocked waiting on msg
volatile PCB *msg_pq[NUM_PRIORITIES];

void message_init(void) {
  semInit(&s);
  semInit(&r);
}

int send_message(int process_ID, void *MessageEnvelope) {
  MSG *msg = (void *)0;
  PCB *dest_proc = (void *) 0;
  
  if (MessageEnvelope == (void *) 0) return -1;
  
  semWait(&s);
  __disable_irq();
  
  // Create the msg
  msg = (MSG *) MessageEnvelope;
  msg->sender_pid = current_process->pid;
  msg->destination_pid = process_ID;
  msg->next = (void *)0;
  // msg->time_stamp = get_current_time();
  
  
  dest_proc = lookup_pid_pq((PCB **)msg_pq, process_ID);
  
  if (dest_proc != (void *)0) {
    enqueue_q(dest_proc->head, msg, MSG_T); // enqueue the msg to the destination_proc's queue
  }
  
  if (dest_proc != (void *)0 && dest_proc->state == BLKD) {
    // Handle unblocking of destination process.
    remove_pq((PCB **)msg_pq, dest_proc);
    dest_proc->state = RDY; // Must be ready to be added to the RDY_Q
    insert_process_pq(dest_proc);
  }
  
  __enable_irq();
  semSignal(&s);
  
  if (dest_proc != (void *)0 && dest_proc->priority < current_process->priority) {
    k_release_processor();
  }
  
  return 0;
}

void *receive_message(int *sender_ID) {
  MSG *msg = (void *) 0;
  semWait(&r);
  __disable_irq();
  
  while(current_process->head == (void *)0) {
    remove_process_pq(current_process);
    current_process->state = BLKD;
    enqueue_q(msg_pq, current_process, PCB_T);
    k_release_processor();
  }
  msg = dequeue_q(current_process->head, MSG_T);
  *sender_ID = msg->sender_pid;
  
  __enable_irq();
  semSignal(&r);
  return msg;
}


