
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define UART_DEVICE "/dev/pts/1"

int uart_init();
int uart_send(int uart_fd, const char *data);
bool uart_receive(int uart_fd, char *buffer, size_t size);
void uart_close(int uart_fd);