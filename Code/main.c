#define _DEBUG_

#include <LPC17xx.h>
#include "uart_polling.h"
#ifdef _DEBUG_
#include <stdio.h>
#endif

#include "memory.h"
#include "process.h"
#include "usr_proc.h"

volatile unsigned int ret_val;

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

void __init()
{
   memory_init();
   process_init();
   __initialize_processes();
}


int main() {
	SystemInit();
  __disable_irq();
 	uart0_init();
  __init();
	__enable_irq();

  
  __set_CONTROL(__get_CONTROL() | BIT(0));  
  ret_val = (unsigned int) request_memory_block();
#ifdef _DEBUG_
	printf("%x\n\r", &Image$$RW_IRAM1$$ZI$$Limit);
#endif
	return 0;
}
