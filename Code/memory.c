#include "uart_polling.h"
#include "memory.h"

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

static unsigned int MEMORY_SIZE;
static int NUM_MEMORY_BLOCKS = 0;

typedef struct MemNode {
	Process *process;
  
  uint8_t block_num;
  unsigned int address;
    
	struct MemNode *next;
} MemNode;

static MemNode* memory_list = 0;
static uint8_t memory_table[NUM_MEMORY_BLOCKS];

void memory_init()
{
  int a;
  for(a = 0; a < NUM_MEMORY_BLOCKS; a++)
    memory_table[a] = 0;
  
  MEMORY_SIZE = (unsigned int) (0x10008000 - (unsigned int) &Image$$RW_IRAM1$$ZI$$Limit);
  NUM_MEMORY_BLOCKS = (MEMORY_SIZE + sizeof(MemNode)) / MEMORY_BLOCK_SIZE;
}

void insert_memory_list(MemNode* mem_node)
{
  if(memory_list == 0)
      memory_list = mem_node;
  else
  {
    MemNode* cur_mem = memory_list;
    while(cur_mem->next != 0)
      cur_mem = cur_mem->next;
    
    cur_mem->next = mem_node;
  }
  
  memory_table[mem_node->block_num] = 1;
}

void remove_memory_list(MemNode* mem_node)
{
  if(memory_list == mem_node)
    memory_list = memory_list->next;
  else
  {
    MemNode* cur_mem = memory_list;
    while(cur_mem->next != mem_node)
      cur_mem = cur_mem->next;
    
    cur_mem->next = mem_node->next;
  }

  memory_table[mem_node->block_num] = 0;    
 // delete mem_node;
}

void* k_request_memory_block(void) {
  unsigned int free_mem = (unsigned int) &Image$$RW_IRAM1$$ZI$$Limit;
  MemNode* mem_node = 0;// = new MemNode;

  
  // Search for free block
  int mem_block_idx = -1, a;
  for(a = 0; a < NUM_MEMORY_BLOCKS; a++)
  {
    if(memory_table[a] == 0)
    {
      mem_block_idx = a;
      break;
    }
  }
  
  // If no memory free, NULL is returned
  if(mem_block_idx == -1)
      return 0;

  mem_node->process = 0;
  mem_node->address = free_mem + mem_block_idx * MEMORY_BLOCK_SIZE;
  mem_node->block_num = mem_block_idx;
  mem_node->next = 0;
  
  insert_memory_list(mem_node);
  return (void *)mem_node->address;
}

int k_release_memory_block(void* p_mem_blk) {
  MemNode* cur_mem = memory_list;
  while(cur_mem != 0)
  {
    if(cur_mem->address == (unsigned int) p_mem_blk)
      break;
    
    cur_mem = cur_mem->next;
  }
  
  if(cur_mem == 0)
     return 1;
  
  remove_memory_list(cur_mem);  
  return 0;
}
