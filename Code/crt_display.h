#ifndef CRT_DISPLAY_H
#define CRT_DISPLAY_H

typedef struct Message {
  void* body;
} Message;

void crt_proc(void* m);

void hot_key_handler(void);
#endif
