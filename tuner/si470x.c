#include "si470x.h"

#include "../i2c.h"
#include "../pins.h"
#include "../functions.h"

static void initRegs()
{

}

void si470xReset(void)
{
    // Reset Si470x pins. TODO: combine with display reset

    OUT_INIT(SI470X_RST, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    IN_P(SI470X_SCLK);
    SET(SI470X_SCLK);
    _delay_ms(1);
    OUT_INIT(SI470X_SDIO, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);

    CLR(SI470X_SDIO);
    CLR(SI470X_RST);
    _delay_ms(1);
    SET(SI470X_RST);
    IN_P(SI470X_SDIO);
    _delay_ms(1);
}

void si470xInit(void)
{
    initRegs();
}
