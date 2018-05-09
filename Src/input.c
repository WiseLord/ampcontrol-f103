#include "input.h"

#include "main.h"
#include "pins.h"

void inputPoll()
{
    static volatile uint32_t i;
    if (i++ & 0x00000100) {
        CLR(DISP_BL);
    } else {
        SET(DISP_BL);
    }
}
