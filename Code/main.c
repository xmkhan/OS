#include <LPC17xx.h>
#include "uart_i_process.h"

#include "memory.h"
#include "process.h"
#include "message.h"
#include "timer.h"
#include "keyboard.h"
#include "crt_display.h"


extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

// Perform intialization of different modules
void __init()
{
   memory_init();
   process_init();
   message_init();
	 keyboard_init();
   crt_init();
	 hotkey_init();
   timer_init(0);
	 wall_clock_init();
}

int main() {
  volatile unsigned int ret_val = 1234;

  // perform initalization
	SystemInit();
  __disable_irq();
 	i_uart_init(0);
  __init();  
	__enable_irq();
  
  __set_CONTROL(__get_CONTROL() | BIT(0)); 
    
  // Output test results
  crt_print("G013_test: START\n\r");
  crt_print("G013_test: total 7 tests\n\r");
  
  ret_val = release_processor();

	return -1;
}
