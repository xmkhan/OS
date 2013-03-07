#ifndef CRT_DISPLAY_H
#define CRT_DISPLAY_H

typedef struct Message {
  void* body;
} Message;

void int_to_char_star(int input, volatile char* buffer);

void crt_proc(void* m);

void crt_output_int(int input);

void hot_key_handler(void);
#endif
