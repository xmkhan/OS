#include "process.h"
#include "uart_polling.h"
#include "usr_proc.h"


#ifdef DEBUG
#include <stdio.h>
#endif  /* DEBUG */


PCB null_pcb;


void __initialize_processes() {
  null_pcb.pid = 0;
  null_pcb.priority = 1;
  null_pcb.state = NEW;
  
  null_proc.pcb = &null_pcb;
  null_proc.start_loc = (uint32_t) null_process;
}

void null_process(void) {
  while(1) {
    int ret_val = release_processor();
#ifdef DEBUG
      printf("\n\rproc1: ret_val=%d. ", ret_val);
#endif
  }
}
