#include "button.h"
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define GPIO1_BASE   0x4804C000
#define MAP_SIZE     0x1000

#define GPIO_OE      0x134
#define GPIO_DATAIN  0x138

#define BUTTON_PIN   16

static volatile uint32_t *gpio_regs;

#define REG32(offset) (*(gpio_regs + ((offset) / 4)))

int button_init()
{
    int fd;
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) return -1;

    void *map_base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_BASE);
    close(fd);

    if (map_base == MAP_FAILED) return -1;

    gpio_regs = (volatile uint32_t *)map_base;

    REG32(GPIO_OE) |= (1 << BUTTON_PIN);

    return 0;
}

int button_read()
{
    return !(REG32(GPIO_DATAIN) & (1 << BUTTON_PIN));
}
