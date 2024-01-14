#include <stdint.h>
#include "uart.h"

unsigned char * uart0 = (unsigned char *)0x10000000; 
#define Reg(reg) ((volatile unsigned char *)(uart0 + reg))
#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

#define RHR 0                 // receive holding register (for input bytes)
#define THR 0                 // transmit holding register (for output bytes)
#define IER 1                 // interrupt enable register
#define IER_RX_ENABLE (1<<0)
#define IER_TX_ENABLE (1<<1)
#define FCR 2                 // FIFO control register
#define FCR_FIFO_ENABLE (1<<0)
#define FCR_FIFO_CLEAR (3<<1) // clear the content of the two FIFOs
#define ISR 2                 // interrupt status register
#define LCR 3                 // line control register
#define LCR_EIGHT_BITS (3<<0)
#define LCR_BAUD_LATCH (1<<7) // special mode to set baud rate
#define LSR 5                 // line status register
#define LSR_RX_READY (1<<0)   // input is waiting to be read from RHR
#define LSR_TX_IDLE (1<<5)    // THR can accept another character to send

void init_uart() {
	WriteReg(IER, IER_RX_ENABLE);
	WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

}

void uart_interrupt() {
	
}

void putchar(char c) {
	WriteReg(THR, c);
	return;
}
char getchar() {
	if (ReadReg(LSR) && LSR_RX_READY) {
		return ReadReg(RHR);	
	}else {
		return -1;
	}
	return -1;
}