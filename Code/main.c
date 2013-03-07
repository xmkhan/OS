#define DEBUG

#include <LPC17xx.h>
#include "uart_i_process.h"
#ifdef DEBUG
#include <stdio.h>
#endif

#include "memory.h"
#include "process.h"
#include "message.h"
#include "crt_display.h"

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

// Perform intialization of different modules
void __init()
{
   memory_init();
   process_init();
   message_init();
}

int main() {
  volatile unsigned int ret_val = 1234;

  // performe initalization
	SystemInit();
  __disable_irq();
 	i_uart_init(0);
  __init();  
	__enable_irq();
  
  __set_CONTROL(__get_CONTROL() | BIT(0)); 
  
  // Output test results
  crt_proc("\n\rG013_test: START");
  crt_proc("\n\rG013_test: total 7 tests");
  
  ret_val = release_processor();

	return -1;
}
