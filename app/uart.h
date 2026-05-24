#ifndef UART_H
#define UART_H

int uart_init(void);

void uart_send_char(char c);

void uart_send_string(const char *str);

void uart_send_log(const char *level, const char *msg);

void uart_close(void);

#endif 
