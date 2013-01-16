#include "uart_polling.h"
#include "memory.h"

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

void* k_request_memory_block(void) {
  unsigned int free_mem = (unsigned int) &Image$$RW_IRAM1$$ZI$$Limit;
  
//	printf("free mem starts at 0x%x\n", free_mem);
  return (void *)0;
}

int k_release_memory_block(void* p_mem_blk) {
  return 0;
}
