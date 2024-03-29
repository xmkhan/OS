/**
 * @brief: uart_irq.c
 * @author: NXP Semiconductors
 * @author: Y. Huang
 * @date: 2013/02/12
 */

#include <LPC17xx.h>
#include "uart_i_process.h"
#include "memory.h"
#include "message.h"
#include "keyboard.h"
#include "crt_display.h"

volatile uint8_t g_UART0_TX_empty=1;
volatile uint8_t g_UART0_buffer[BUFSIZE];
volatile uint32_t g_UART0_count = 0;
PCB *saved_process;
MSG *hotkey_msg = (void *)0;


/**
 * @brief: initialize the n_uart
 * NOTES: only fully supports uart0 so far, but can be easily extended
 *        to other uarts.
 *        The step number in the comments matches the item number
 *        in Section 14.1 on pg 298 of LPC17xx_UM
 */
int i_uart_init(int n_uart) {

	LPC_UART_TypeDef *pUart;

	if (n_uart ==0 ) {
		/*
		Steps 1 & 2: system control configuration.
		Under CMSIS, system_LPC17xx.c does these two steps

		-----------------------------------------------------
		Step 1: Power control configuration.
		        See table 46 pg63 in LPC17xx_UM
		-----------------------------------------------------
		Enable UART0 power, this is the default setting
		done in system_LPC17xx.c under CMSIS.
		Enclose the code for your refrence
		//LPC_SC->PCONP |= BIT(3);

		-----------------------------------------------------
		Step2: Select the clock source.
		       Default PCLK=CCLK/4 , where CCLK = 100MHZ.
		       See tables 40 & 42 on pg56-57 in LPC17xx_UM.
		-----------------------------------------------------
		Check the PLL0 configuration to see how XTAL=12.0MHZ
		gets to CCLK=100MHZin system_LPC17xx.c file.
		PCLK = CCLK/4, default setting after reset.
		Enclose the code for your reference
		//LPC_SC->PCLKSEL0 &= ~(BIT(7)|BIT(6));

		-----------------------------------------------------
		Step 5: Pin Ctrl Block configuration for TXD and RXD
		        See Table 79 on pg108 in LPC17xx_UM.
		-----------------------------------------------------
		Note this is done before Steps3-4 for coding purpose.
		*/

		/* Pin P0.2 used as TXD0 (Com0) */
		LPC_PINCON->PINSEL0 |= (1 << 4);

		/* Pin P0.3 used as RXD0 (Com0) */
		LPC_PINCON->PINSEL0 |= (1 << 6);

		pUart = (LPC_UART_TypeDef *) LPC_UART0;

	} else if (n_uart == 1) {

		/* see Table 79 on pg108 in LPC17xx_UM */
		/* Pin P2.0 used as TXD1 (Com1) */
		LPC_PINCON->PINSEL0 |= (2 << 0);

		/* Pin P2.1 used as RXD1 (Com1) */
		LPC_PINCON->PINSEL0 |= (2 << 2);

		pUart = (LPC_UART_TypeDef *) LPC_UART1;

	} else {
		return 1; /* not supported yet */
	}

	/*
	-----------------------------------------------------
	Step 3: Transmission Configuration.
	        See section 14.4.12.1 pg313-315 in LPC17xx_UM
	        for baud rate calculation.
	-----------------------------------------------------
        */

	/* Step 3a: DLAB=1, 8N1 */
	pUart->LCR = UART_8N1; /* see uart.h file */

	/* Step 3b: 115200 baud rate @ 25.0 MHZ PCLK */
	pUart->DLM = 0; /* see table 274, pg302 in LPC17xx_UM */
	pUart->DLL = 9;	/* see table 273, pg302 in LPC17xx_UM */

	/* FR = 1.507 ~ 1/2, DivAddVal = 1, MulVal = 2
	   FR = 1.507 = 25MHZ/(16*9*115200)
	   see table 285 on pg312 in LPC_17xxUM
	*/
	pUart->FDR = 0x21;



	/*
	-----------------------------------------------------
	Step 4: FIFO setup.
	       see table 278 on pg305 in LPC17xx_UM
	-----------------------------------------------------
        enable Rx and Tx FIFOs, clear Rx and Tx FIFOs
	Trigger level 0 (1 char per interrupt)
	*/

	pUart->FCR = 0x07;

	/* Step 5 was done between step 2 and step 4 a few lines above */

	/*
	-----------------------------------------------------
	Step 6 Interrupt setting and enabling
	-----------------------------------------------------
	*/
	/* Step 6a:
	   Enable interrupt bit(s) wihtin the specific peripheral register.
           Interrupt Sources Setting: RBR, THRE or RX Line Stats
	   See Table 50 on pg73 in LPC17xx_UM for all possible UART0 interrupt sources
	   See Table 275 on pg 302 in LPC17xx_UM for IER setting
	*/
	/* disable the Divisior Latch Access Bit DLAB=0 */
	pUart->LCR &= ~(BIT(7));

	pUart->IER = IER_RBR | IER_THRE | IER_RLS;

	/* Step 6b: enable the UART interrupt from the system level */
	NVIC_EnableIRQ(UART0_IRQn); /* CMSIS function */

  // Allocate memory for keyboard so that when memory runs out keyboard features used for debugging still work
  hotkey_msg = (MSG*) k_request_memory_block();
	hotkey_msg->msg_type = 2;

	return 0;
}

/**
 * @brief: use CMSIS ISR for UART0 IRQ Handler
 * NOTE: This example shows how to save/restore all registers rather than just
 *       those backed up by the exception stack frame. We add extra
 *       push and pop instructions in the assembly routine.
 *       The actual c_UART0_IRQHandler does the rest of irq handling
 */
__asm void UART0_IRQHandler(void)
{
	PRESERVE8
	IMPORT c_UART0_IRQHandler
	PUSH{r4-r11, lr}
	BL c_UART0_IRQHandler
	POP{r4-r11, pc}
}
/**
 * @brief: c UART0 IRQ Handler
 */
void c_UART0_IRQHandler(void)
{
	uint8_t IIR_IntId;      /* Interrupt ID from IIR */
	uint8_t LSR_Val;        /* LSR Value             */
	uint8_t dummy = dummy;	/* to clear interrupt upon LSR error */
	uint8_t input_char;
	PCB* saved_process = (void *)0;
	LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;
  volatile int iState = k_get_interrupt_state();

	k_set_interrupt_state(4);

	/* Reading IIR automatically acknowledges the interrupt */
	IIR_IntId = (pUart->IIR) >> 1 ; /* skip pending bit in IIR */

	if (IIR_IntId & IIR_RDA) { /* Receive Data Avaialbe */
		/* read UART. Read RBR will clear the interrupt */
		input_char = pUart->RBR;

		if (input_char == 96) {
			saved_process = current_process;
			hotkey_msg->msg_data = (void *)saved_process;
			k_send_message(HOTKEY_PID, hotkey_msg);
			k_context_switch(hotkey_pcb);
		} else {
			saved_process = current_process;
			// check if at start the timer started before processes started being schedule, if so no context switch
			if(!(saved_process->pid == 0 && saved_process->state == NEW))
				k_context_switch(keyboard_pcb);

			keyboard_proc(input_char, saved_process);

			// check if at start the timer started before processes started being schedule, if so no context switch
			if(!(saved_process->pid == 0 && saved_process->state == NEW))
				k_context_switch(saved_process);

			g_UART0_buffer[g_UART0_count++] = input_char;

			if ( g_UART0_count == BUFSIZE ) {
				//g_UART0_buffer[g_UART0_count] = '\0';
				//keyboard_proc((char *)g_UART0_buffer);
				g_UART0_count = 0;  /* buffer overflow */
			}
		}

	} else if (IIR_IntId & IIR_THRE) {
		/* THRE Interrupt, transmit holding register empty*/

		LSR_Val = pUart->LSR;
		if(LSR_Val & LSR_THRE) {
			g_UART0_TX_empty = 1; /* ready to transmit */
		} else {
			g_UART0_TX_empty = 0; /* not ready to transmit */
		}

	} else if (IIR_IntId & IIR_RLS) {
		LSR_Val = pUart->LSR;
		if (LSR_Val  & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) {
			/* There are errors or break interrupt
		           Read LSR will clear the interrupt
			   Dummy read on RX to clear interrupt, then bail out
			*/
			dummy = pUart->RBR;
      k_set_interrupt_state(iState);
			return; /* error occurs, return */
		}
		/* If no error on RLS, normal ready, save into the data buffer.
	           Note: read RBR will clear the interrupt
		*/
		if (LSR_Val & LSR_RDR) { /* Receive Data Ready */
			/* read from the uart */
			g_UART0_buffer[g_UART0_count++] = pUart->RBR;

			if ( g_UART0_count == BUFSIZE ) {
				g_UART0_count = 0;  /* buffer overflow */
			}
		}
	} else { /* IIR_CTI and reserved combination are not implemented */
    k_set_interrupt_state(iState);
		return;
	}

  k_set_interrupt_state(iState);
}

void uart_i_process( uint32_t n_uart, uint8_t *p_buffer, uint32_t len )
{
	LPC_UART_TypeDef *pUart;

	if(n_uart == 0 ) { /* UART0 is implemented */
		pUart = (LPC_UART_TypeDef *)LPC_UART0;
	} else { /* other UARTs are not implemented */
		return;
	}

	pUart->IER = IER_THRE;
	while ( len != 0 ) {
		/* THRE status, contain valid data  */
		while ( !(g_UART0_TX_empty & 0x01) );
		pUart->THR = *p_buffer;
		g_UART0_TX_empty = 0;  // not empty in the THR until it shifts out
		p_buffer++;
		len--;
	}
	pUart->IER = IER_RBR | IER_THRE | IER_RLS;
	return;
}

