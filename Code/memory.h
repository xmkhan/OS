#ifndef MEMORY_H
#define MEMORY_H

#include "process.h"

#ifndef __SVC_0
#define __SVC_0  __svc_indirect(0)
#endif

// Configuration parameters for memory
#define MEMORY_BLOCK_SIZE 128
#define MEMORY_BLOCK_SIZE_HEX 0x80

extern volatile PCB *mem_pq[];

void memory_init(void);

void* k_request_memory_block(void);
#define request_memory_block() _request_memory_block((unsigned int)k_request_memory_block)
extern void* _request_memory_block(unsigned int p_func) __SVC_0;

extern int k_release_memory_block(void *);
#define release_memory_block(p_mem_blk) _release_memory_block((unsigned int)k_release_memory_block, p_mem_blk)
extern int _release_memory_block(unsigned int p_func, void * p_mem_blk) __SVC_0;

#endif
