#ifndef MSG_H
#define MSG_H

// Message structure that encapsulates complements of the MSG
typedef struct MSG {
    int sender_pid;
    int destination_pid;
    int msg_type; // 0 = empty, ...
    void *msg_data;
    struct MSG *next;
    long time_stamp;
} MSG;

/**
 * Initializes the memory module
 */
void message_init(void);

/**
 * API: send_message (non-blocking)
 * Call to send a message from one process to the mailbox (queue) of another
 * @param process_ID PID of the destination
 * @param MessageEnvelope MEM_BLOCK containing the (MSG *) structure
 */
int send_message(int process_ID, void *MessageEnvelope);

/**
 * API: receive_message (blocking
 * Blocks the process until the requiring message arrives in it's mailbox (queue)
 * @param sender_ID PID of the sender, used for output
 */
void *receive_message(int *sender_ID);

/**
 * API: delay_send (non-blocking)
 * Call to send a message from one process to the mailbox (queue) of another after a certain delay
 * @param process_ID PID of the destination
 * @param MessageEnvelope MEM_BLOCK containing the (MSG *) structure
 * @param delay After how many milliseconds to send the message
 */
int delayed_send(int process_ID, void *MessageEnvelope, int delay);

#endif
