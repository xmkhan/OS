#ifndef MEMORY_H
#define MEMORY_H

#include "process.h"

#ifndef __SVC_0
#define __SVC_0  __svc_indirect(0)
#endif

// Configuration parameters for memory
#define MEMORY_BLOCK_SIZE 512
// TODO: remove exposure
extern int NUM_MEMORY_BLOCKS;

// MemNode struct which contains information about the memory block,
// and is stored as a memory at the start of each memory block
typedef struct MemNode {
  uint8_t block_num;
  uint8_t used;
  unsigned int address;
    
	struct MemNode *next;
} MemNode;


// Initialize memory module
void memory_init(void);

/**
 * API: request_memory_block
 * Call to request block of memory
 * Process gets blocked if there is no memory available
 */
void* k_request_memory_block(void);
#define request_memory_block() _request_memory_block((unsigned int)k_request_memory_block)
extern void* _request_memory_block(unsigned int p_func) __SVC_0;

/**
 * API: release_memory_block
 * Call to release allocated block of memory for other processes to use
 */
int k_release_memory_block(void *);
#define release_memory_block(p_mem_blk) _release_memory_block((unsigned int)k_release_memory_block, p_mem_blk)
extern int _release_memory_block(unsigned int p_func, void * p_mem_blk) __SVC_0;

#endif
