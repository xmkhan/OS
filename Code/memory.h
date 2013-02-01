#ifndef MEMORY_H
#define MEMORY_H

#include "process.h"

#ifndef __SVC_0
#define __SVC_0  __svc_indirect(0)
#endif

// Configuration parameters for memory
#define MEMORY_BLOCK_SIZE 128
#define MEMORY_BLOCK_SIZE_HEX 0x80

//Stores PCB's of processes which are currently blocked due to memory request event
extern volatile PCB *mem_pq[];

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
extern int k_release_memory_block(void *);
#define release_memory_block(p_mem_blk) _release_memory_block((unsigned int)k_release_memory_block, p_mem_blk)
extern int _release_memory_block(unsigned int p_func, void * p_mem_blk) __SVC_0;

#endif
