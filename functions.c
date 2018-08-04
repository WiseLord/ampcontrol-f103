#include "functions.h"

void _delay_us(uint32_t us)
{
    volatile uint32_t n = 8 * us;

    while (--n > 0);
}

void _delay_ms(uint32_t ms)
{
    volatile uint32_t n = 1000;

    while (--n > 0) {
        _delay_us(ms);
    }
}
