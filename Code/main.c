#include <LPC17xx.h>
#include "uart_polling.h"
//#include <stdio.h>
#include "memory.h"

volatile unsigned int ret_val;

int main() {
	SystemInit();
  
  __set_CONTROL(__get_CONTROL() | BIT(0));  

  ret_val = (unsigned int) request_memory_block();
	return 0;
}
