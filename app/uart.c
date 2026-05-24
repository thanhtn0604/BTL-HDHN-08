#include "uart.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <time.h>

static int uart_fd = -1;

int uart_init(void)
{
    struct termios opts;
    
    uart_fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY);
    if (uart_fd < 0) return -1;
    
    tcgetattr(uart_fd, &opts);
    
    cfsetispeed(&opts, B115200);
    cfsetospeed(&opts, B115200);
    
    opts.c_cflag |= (CLOCAL | CREAD);
    opts.c_cflag &= ~PARENB;
    opts.c_cflag &= ~CSTOPB;
    opts.c_cflag &= ~CSIZE;
    opts.c_cflag |= CS8;
    
    opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    opts.c_iflag &= ~(IXON | IXOFF | IXANY);
    opts.c_oflag &= ~OPOST;
    
    tcsetattr(uart_fd, TCSANOW, &opts);
    
    return 0;
}

void uart_send_char(char c)
{
    if (uart_fd >= 0) write(uart_fd, &c, 1);
}

void uart_send_string(const char *str)
{
    if (uart_fd >= 0 && str) write(uart_fd, str, strlen(str));
}

void uart_send_log(const char *level, const char *msg)
{
    char buf[256];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    snprintf(buf, sizeof(buf), "[%02d:%02d:%02d] [%s] %s\r\n", 
             t->tm_hour, t->tm_min, t->tm_sec, level, msg);
             
    uart_send_string(buf);
}

void uart_close(void)
{
    if (uart_fd >= 0) {
        close(uart_fd);
        uart_fd = -1;
    }
}
