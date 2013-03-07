#include "pq.h"
#include "message.h"
#include "semaphore.h"
#include "timer.h"

semaphore send;
semaphore receive;

// PQ storing PCBs which are blocked waiting on msg
volatile PCB *msg_pq[NUM_PRIORITIES];

void message_init(void) {
  semInit(&send);
  semInit(&receive);
}

MSG *get_message_pid(int process_ID) {
  PCB *pcb = lookup_pid(process_ID);
  if (pcb != (void *)0) {
    dequeue_q(pcb->head, MSG_T);
    return pcb->head;
  }
  return (void *)0;
}

MSG *get_message(PCB *pcb) {
  if (pcb != (void *)0) {
    dequeue_q(pcb->head, MSG_T);
    return pcb->head;
  }
  return (void *)0;
}

int send_message_global(int dest_process_ID, void *MessageEnvelope, int router_process_pid, long delay) {
  MSG *msg = (void *)0;
  PCB *dest_proc = (void *) 0;

  if (MessageEnvelope == (void *) 0) return -1;

  semWait(&send);
  __disable_irq();

  // Create the msg
  msg = (MSG *) MessageEnvelope;
  if (current_process->pid != TIMER_PID) {
    msg->sender_pid = current_process->pid;
  }
  msg->destination_pid = dest_process_ID;
  msg->next = (void *)0;
  msg->expiry_time = get_current_time() + delay;


  dest_proc = lookup_pid(router_process_pid);

  // If it doesn't exist in our ready queue, check BLKD msg queue
  if (dest_proc == (void *)0) {
    dest_proc = lookup_pid_pq((PCB **)msg_pq, router_process_pid);
  }

  if (dest_proc != (void *)0) {
    enqueue_q(dest_proc->head, msg, MSG_T); // enqueue the msg to the destination_proc's queue
  }

  if (dest_proc != (void *)0 && dest_proc->state == BLKD && delay == 0) {
    // Handle unblocking of destination process.
    remove_pq((PCB **)msg_pq, dest_proc);
    dest_proc->state = RDY; // Must be ready to be added to the RDY_Q
    insert_process_pq(dest_proc);
  }

  __enable_irq();
  semSignal(&send);

  if (dest_proc != (void *)0 && dest_proc->priority < current_process->priority && delay == 0) {
    k_release_processor(); // Destination process has a higher priority, release processor
  }

  return 0;
}

int send_message(int process_ID, void *MessageEnvelope)
{
  return send_message_global(process_ID, MessageEnvelope, process_ID, 0);
}

int delayed_send(int process_ID, void *MessageEnvelope, int delay)
{
  if(delay <= 0)
    return -1;
  
  return send_message_global(process_ID, MessageEnvelope, TIMER_PID, delay);
}


void *receive_message(int *sender_ID) {
  MSG *msg = (void *) 0;
  PCB *dest_proc = (void *)0;

  semWait(&receive);
  __disable_irq();

  while(current_process->head == (void *)0) { // We have yet to receive any msgs
    // Move process from ready_queue to blocking_msg_queue
    dest_proc = lookup_pid_pq((PCB **)msg_pq, current_process->pid);
    if (dest_proc == (void *)0) { // PCB has not been added to the blocking msg_pq
      remove_process_pq(current_process);
      current_process->state = BLKD; // Set state to BLKD
      enqueue_q(msg_pq, current_process, PCB_T);
    }
    k_release_processor(); // Release this process as it is blocked
  }
  msg = dequeue_q(current_process->head, MSG_T); // We have acquired a 'msg'
  *sender_ID = msg->sender_pid; // Fill in the sender_ID

  __enable_irq();
  semSignal(&receive);

  return msg;
}
