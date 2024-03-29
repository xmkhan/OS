#ifndef MSG_H
#define MSG_H

#ifndef __SVC_0
#define __SVC_0  __svc_indirect(0)
#endif

typedef struct PCB PCB;

// Message structure that encapsulates complements of the MSG
typedef struct MSG {
    int sender_pid;
    int destination_pid;
    int msg_type; // 0 = empty, 1 = int, 2 = kernel lvl messages, 3 = count_report, 4 = command registration, 5 = wakeup10...
    void *msg_data;
    struct MSG *next;
    long expiry_time;
} MSG;

extern volatile PCB *msg_pq[];

/**
 * Initializes the memory module
 */
void message_init(void);

/**
 * Non-blocking receive used as a lookup for the queue
 * @return If mailbox (queue) empty return (void *)0, else MSG *
 */
MSG *get_message_pid(int process_ID);

MSG *k_get_message(PCB *);
#define get_message(pcb) _get_message((unsigned int) k_get_message, pcb)
extern MSG *_get_message(unsigned int p_func, PCB *pcb) __SVC_0;

/**
 * API: send_message (non-blocking)
 * Call to send a message from one process to the mailbox (queue) of another
 * @param process_ID PID of the destination
 * @param MessageEnvelope MEM_BLOCK containing the (MSG *) structure
 */
int k_send_message(int , void *);
#define send_message(process_ID, MessageEnvelope) _send_message((unsigned int) k_send_message, process_ID, MessageEnvelope)
extern int _send_message(unsigned int p_func, int process_ID, void *MessageEnvelope) __SVC_0;

/**
 * API: receive_message (blocking
 * Blocks the process until the requiring message arrives in it's mailbox (queue)
 * @param sender_ID PID of the sender, used for output
 */
void *k_receive_message(int *);
#define receive_message(sender_ID) _receive_message((unsigned int) k_receive_message, sender_ID)
extern void* _receive_message(unsigned int p_func, int *sender_ID) __SVC_0;

/**
 * API: delay_send (non-blocking)
 * Call to send a message from one process to the mailbox (queue) of another after a certain delay
 * @param process_ID PID of the destination
 * @param MessageEnvelope MEM_BLOCK containing the (MSG *) structure
 * @param delay After how many milliseconds to send the message
 */
int k_delayed_send(int , void *, int);
#define delayed_send(process_ID, MessageEnvelope, delay) _delayed_send((unsigned int) k_delayed_send, process_ID, MessageEnvelope, delay)
extern int _delayed_send(unsigned int p_func, int process_ID, void *MessageEnvelope, int delay) __SVC_0;

#endif
