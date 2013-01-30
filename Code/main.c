#define DEBUG

#include <LPC17xx.h>
#include "uart_polling.h"
#ifdef DEBUG
#include <stdio.h>
#endif

#include "memory.h"
#include "process.h"

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

void __init()
{
   memory_init();
   process_init();
}

int main() {
  volatile unsigned int ret_val = 1234;

	SystemInit();
  __disable_irq();
 	uart0_init();
  __init();
	__enable_irq();

  __set_CONTROL(__get_CONTROL() | BIT(0));  
  ret_val = release_processor();

	return -1;
}
