#define _DEBUG_

#include <LPC17xx.h>
#include "uart_polling.h"
#ifdef _DEBUG_
#include <stdio.h>
#endif
#include "memory.h"

MemNodeHead m;
volatile unsigned int ret_val;

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;


int main() {
	SystemInit();
  __disable_irq();
	uart0_init();
	__enable_irq();
	
  __set_CONTROL(__get_CONTROL() | BIT(0));  

  ret_val = (unsigned int) request_memory_block();
#ifdef _DEBUG_
	printf("%x\n\r", &Image$$RW_IRAM1$$ZI$$Limit);
#endif
	return 0;
}
