#ifndef MSG_H
#define MSG_H

typedef struct MSG {
    int sender_pid;
    int destination_pid;
    int msg_type; // 0 = empty, ...
    void *msg_data;
    struct MSG *next;
    long time_stamp;
} MSG;


void message_init(void);

int send_message(int process_ID, void *MessageEnvelope);
void *receive_message(int *sender_ID);

#endif
