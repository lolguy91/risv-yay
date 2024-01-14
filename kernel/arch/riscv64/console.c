#include "uart.h"

void console_write(char c) {
    putchar(c);
    return;
}
char console_read() {
    return getchar();
}