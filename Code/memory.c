#include "uart_polling.h"
#include "memory.h"

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

static unsigned int MEMORY_SIZE;
static int NUM_MEMORY_BLOCKS;

typedef struct MemNode {
  uint8_t block_num;
  uint8_t used;
  unsigned int address;
    
	struct MemNode *next;
} MemNode;

static MemNode* memory_list = 0;

void memory_init()
{
  volatile int a;
  MemNode* mem_node;

  MEMORY_SIZE = (unsigned int) ((unsigned) 0x10008000 - (unsigned int) &Image$$RW_IRAM1$$ZI$$Limit);
  NUM_MEMORY_BLOCKS = MEMORY_SIZE / (MEMORY_BLOCK_SIZE + sizeof(MemNode));

  memory_list = (MemNode *)((unsigned int)&Image$$RW_IRAM1$$ZI$$Limit);
  mem_node = memory_list;
  for(a=0; a < NUM_MEMORY_BLOCKS; a++)
  {
    mem_node->block_num = a;
    mem_node->used = 0;
    mem_node->address = (unsigned int)mem_node + sizeof(MemNode);
    mem_node->next = (MemNode*) (mem_node->address + MEMORY_BLOCK_SIZE);
    
    mem_node = mem_node->next;
  } 
}

void* k_request_memory_block(void) {
  MemNode* mem_node = memory_list;
  
  // Search for free block
  while(mem_node != 0)
  {
    // set block as used
    if(mem_node->used == 0)
    {
      mem_node->used = 1;
      return (void *)mem_node->address;
    }
     
    mem_node = mem_node->next;
  }

  return 0;
}

int k_release_memory_block(void* p_mem_blk) {
  MemNode* cur_mem = memory_list;
  while(cur_mem != 0)
  {
    if(cur_mem->address == (unsigned int) p_mem_blk)
    {
      cur_mem->used = 0;
      return 0;
    }
    
    cur_mem = cur_mem->next;
  }
  
  return 1;
}
