#include "pq.h"
#include "message.h"

#ifndef NULL
#define NULL (void *)0
#endif

/**
 * Inserts the PCB into the PQ
 * @param  pq [Priority Queue]
 * @param  p  [pcb]
 * @return    [0 on success, -1 on failure]
 */
int insert_pq(PCB* pq[], PCB* p) {
  volatile PCB *pr_head = NULL;
    if (p == NULL || p->priority >= NUM_PRIORITIES) {
        return -1;
    }

    pr_head = pq[p->priority];
    if (pr_head == NULL){
      pq[p->priority] = p;
    } else {
      while(pr_head->next != NULL)
      {
        pr_head = pr_head->next;
      }
      pr_head->next = p;
      p->next = NULL;
    }
    return 0;
}

/**
 * Removes the PCB into the PQ
 * @param  pq [Priority Queue]
 * @param  p  [pcb]
 * @return    [0 on success, -1 on failure]
 */
int remove_pq(PCB* pq[], PCB* p) {
  volatile PCB *pr_head = NULL;
  if (p == NULL || p->priority >= NUM_PRIORITIES) {
      return -1;
  }

  pr_head = pq[p->priority];
  if (pr_head != p)
  {
    while(pr_head != (void *)0 && pr_head->next != p) {
        pr_head = pr_head->next;
    }
    if (pr_head == (void *)0) return 1;
    pr_head->next = p->next;
  } else
  {
    pq[p->priority] = pr_head->next;
    //pr_head->pcb->state = EXIT;
  }
  return 0;
}

/**
 * Get next available process
 * @param  pq [Priority Queue]
 * @return    [PCB* on success, NULL on error]
 */
PCB* get_process(PCB *pq[]) {
  volatile unsigned int i = 0;
  volatile PCB *pr_head = NULL;
  for(; i < NUM_PRIORITIES; ++i)
  {
     pr_head = pq[i];
     while (pr_head != NULL)
     {
       pr_head = pr_head->next;
     }
     if (pr_head != NULL) return (PCB*) pr_head;
  }
  return 0; // return NULL
}

/**
 * Lookup a PCB using its PID
 * @param  pq  [Priority Queue]
 * @param  pid [Process Identifier]
 * @return     [PCB* on success, NULL on error]
 */
PCB *lookup_pid_pq(PCB* pq[], int pid) {
  volatile unsigned int i = 0;
  volatile PCB *pr_head = NULL;
  for(; i < NUM_PRIORITIES; ++i)
  {
     pr_head = pq[i];
     while (pr_head != NULL && pr_head->pid != pid)
     {
       pr_head = pr_head->next;
     }
     if (pr_head != NULL) return (PCB *)pr_head;
  }
  return (void *)0;
}

int enqueue_q(void* pq_generic, void* p_generic, q_type type) {
  if (type == PCB_T) {
    PCB **pq = (PCB **) pq_generic;
    PCB *p = (PCB *) p_generic;
    PCB *head = *pq;
    p->next = NULL; // Set entry to be added to NULL
    if (head == NULL) { // Base case, empty queue
      *pq = p;
      return 0;
    }
    while (head->next != NULL) { // Traverse to the end
      head = head->next;
    }
    head->next = p;
} else if (type == MSG_T) {
    MSG **pq = (MSG **) pq_generic;
    MSG *p = (MSG *) p_generic;
    MSG *head = *pq;
      ((MSG *)p_generic)->next = (void *)0;
    p->next = (void *)0; // Set entry to be added to NULL
    if (head == NULL) { // Base case, empty queue
      *pq = p;
      return 0;
    }
    while (head->next != NULL) { // Traverse to the end
      head = head->next;
    }
    head->next = p;
} else if(type == DLY_MSG_T)
{
    MSG **pq = (MSG **) pq_generic;
    MSG *p = (MSG *) p_generic;
    MSG *head = *pq;
    p->next = NULL; // Set entry to be added to NULL
    if (head == NULL) { // Base case, empty queue
      *pq = p;
      return 0;
    }
    while (head->next != NULL && head->next->expiry_time < p->expiry_time) { // Traverse to the end
      head = head->next;
    }
    head->next = p;  
}

  return 0;
}


void *dequeue_q(void* pq_generic, q_type type) {
  if (type == PCB_T) {
    PCB **pq = (PCB **) pq_generic;
    PCB *p = *pq; 
    *pq = (*pq)->next; // Move the head to the next element
    return p;
  } else if (type == MSG_T) {
    MSG **pq = (MSG **) pq_generic;
    MSG *p = *pq;
    *pq = (*pq)->next; // Move the head to the next element
    return p;
  }
  
  return (void *)0;
}
