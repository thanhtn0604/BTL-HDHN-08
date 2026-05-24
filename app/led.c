#include "led.h"

#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define GPIO1_BASE          0x4804C000
#define MAP_SIZE            0x1000

#define GPIO_OE             0x134
#define GPIO_DATAOUT        0x13C
#define GPIO_SETDATAOUT     0x194
#define GPIO_CLEARDATAOUT   0x190

#define LED_PIN             28

static volatile uint32_t *gpio_regs;

#define REG32(offset)       (*(gpio_regs + ((offset) / 4)))

int led_init()
{
    int fd;

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0)
        return -1;

    void *map_base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_BASE);
    close(fd); 

    if (map_base == MAP_FAILED)
        return -1;

    gpio_regs = (volatile uint32_t *)map_base;

    REG32(GPIO_OE) &= ~(1 << LED_PIN);

    return 0;
}

void led_on()
{
    REG32(GPIO_SETDATAOUT) = (1 << LED_PIN);
}

void led_off()
{
    REG32(GPIO_CLEARDATAOUT) = (1 << LED_PIN);
}

void led_toggle()
{
    if (REG32(GPIO_DATAOUT) & (1 << LED_PIN))
        led_off();
    else
        led_on();
}
