#include <LPC17xx.h>
#include "uart_i_process.h"
#ifdef DEBUG
#include <stdio.h>
#endif

#include "memory.h"
#include "process.h"
#include "message.h"
#include "timer.h"

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

// Perform intialization of different modules
void __init()
{
   memory_init();
   process_init();
   message_init();
   timer_init(0);
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
#ifdef DEBUG
	printf("\n\rG013_test: START");
	printf("\n\rG013_test: total 7 tests");
#else
	//uart0_put_string("\n\rG013_test: START");
	//uart0_put_string("\n\rG013_test: total 7 tests");
#endif
  ret_val = release_processor();

	return -1;
}
