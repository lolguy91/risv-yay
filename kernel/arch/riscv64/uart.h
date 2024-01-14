#ifndef UART_H 
#define UART_H

#include <stdint.h>
#include <stddef.h>

void init_uart();
void uart_interrupt();

void putchar(char c);
char getchar();

#endif 