#include "crt_display.h"
#include "uart_i_process.h"
#include "process.h"
#include "usr_proc.h"

#define NUM_STATES  5
#define COL_SIZE    10 
#define BUFFER_SIZE 10

static char buffer[BUFFER_SIZE];

/**
 * System function available to processes
 * for non-blocking output to console
 * @param m - the message to be printed
 */
void crt_proc(char* m)
{
  volatile int length = 0;
  char* b = m;
  
  if(!b) return;
  
  //Find the length of the message
  while(*b != '\0') {
    b++;
    length++;
  }
  
  if(length == 0) return;
  
  //non-blocking output
  uart_i_process( 0, (uint8_t* ) m, length );
}

/**
 * Convert a positive integer or 0 to char*
 * @param buffer - stores char* representation
 *        input  - int to be converted
 */
void int_to_char_star(int input, volatile char* b) {
  
  //case for 0;
  if(input == 0) {
    *b = '0';
    b++;
  }
  
  //case for positive number
  while(input > 0) {
    *b = (char)(((int)'0')+input%10);
    input /= 10;
    b++;
  }
  
  //add null terminating character
  *b   = '\0';
}

void crt_output_int(int input) {
  int_to_char_star(input, buffer);
  crt_proc(buffer);
}

/*
 * handle hot-key key press 
 */
void hot_key_handler(void) {
  char* header = "\n\rProc_id  Priority Status\n\r";
  int i = 0, j=0, n = 0, col_empty = 0;
  PCB *iterate;  
  
  //States' char* representation
  char* p_states[NUM_STATES];
  p_states[j++] = "New";
  p_states[j++] = "Ready";
  p_states[j++] = "Running";
  p_states[j++] = "Blocked";
  p_states[j++] = "Exit"; 

  //the categories we would be outputing
  crt_proc((void*)header);
  
  //iterate through every process
  //and output relevant information
  for(;i < NUM_PROCESSES; i++) {
    iterate = pcb_list[i];
    
    //output the process id
    int_to_char_star(i, buffer);
    n = i/10 + 1;
    col_empty = COL_SIZE - n;
    for(;n < col_empty; n++) {
      buffer[n] = ' ';
    }
    crt_proc((void*)buffer);
    
    //output the priority
    int_to_char_star(iterate->priority, buffer);
    n = iterate->priority/10 + 1;
    col_empty = COL_SIZE - n;
    for(;n < col_empty; n++) {
      buffer[n] = ' ';
    }
    
    //output the state
    crt_proc((void*)buffer);
    crt_proc((void*) p_states[iterate->state]);
    
    //Print a new line at the end
    buffer[0] = '\n';
    buffer[1] = '\r';
    buffer[2] = '\0';
    crt_proc((void*) buffer);
  }
}

