#include "display.h"

#if defined (_KS0108B)
#include "display/ks0108.h"
#elif defined (_ILI9320)
#include "display/ili9320.h"
#else
#error "Unsupported display driver"
#endif

#include "actions.h"

static DisplayDriver *disp;

static int8_t brStby;
static int8_t brWork;

void displayInit()
{
#if defined (_KS0108B)
    ks0108Init(&disp);

    // TODO: Read from backup memory
    brStby = 1;
    brWork = 30;

    displayChangeBrighness(AMODE_BRIGNTNESS_STANDBY, brStby);
#elif defined (_ILI9320)
    ILI9320_Init(&disp);
    ILI9320_Rotate(LCD_Orientation_Landscape_1);

    ILI9320_DrawFilledRectangle(0, 0, 319, 239, LCD_COLOR_BLACK);
#endif

}

void displayClear()
{
#if defined(_GM128X64)
    ks0108Clear();
#endif
}

void displayUpdateIRQ()
{
#if defined(_KS0108B)
    ks0108IRQ();
#elif defined(_ILI9320)
    ILI9320_IRQ();
#endif
}

int8_t displayGetBrightness(uint8_t mode)
{
    if (mode == AMODE_BRIGHTNESS_WORK)
        return brWork;
    else
        return brStby;
}

void displayChangeBrighness(uint8_t mode, int8_t diff)
{
    int8_t *br = &brStby;

    if (mode == AMODE_BRIGHTNESS_WORK)
        br = &brWork;

    *br += diff;

    // TODO: Use param from driver/layout
    if (*br > 32)
        *br = 32;
    if (*br < 0)
        *br = 0;

    if (disp->setBrightness) {
        disp->setBrightness(*br);
    }
}

uint8_t displayReadBus(void)
{
    return disp->readBus ? disp->readBus() : 0;
}

void displayShowTime(RTC_type *rtc, char *wday)
{
    if (disp->layout->showTime) {
        disp->layout->showTime(rtc, wday);
    }
}

void displayShowParam(DispParam *dp)
{
    if (disp->layout->showParam) {
        disp->layout->showParam(dp);
    }
}

void displayShowSpectrum(uint8_t *dataL, uint8_t *dataR)
{
    if (disp->layout->showSpectrum) {
        disp->layout->showSpectrum(dataL, dataR);
    }
}
