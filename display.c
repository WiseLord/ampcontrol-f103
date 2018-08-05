#include "display.h"

#if defined(_GC320X240)
#include "display/gc320x240.h"
#else
#include "display/gm128x64.h"
#include "display/gdfb.h"
#endif
#include "actions.h"

static Display *disp;

static int8_t brStby;
static int8_t brWork;

static char strbuf[STR_BUFSIZE + 1];   // String buffer

void displayInit()
{
#if defined(_GC320X240)
    gc320x240Init(&disp);
#else
    gm128x64Init(&disp);

    // TODO: Read from backup memory
    brStby = GD_MAX_BRIGHTNESS / 16;
    brWork = GD_MAX_BRIGHTNESS;

    displayChangeBrighness(AMODE_BRIGNTNESS_STANDBY, brStby);
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
#if defined(_GM128X64)
    ks0108IRQ();
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

    if (*br > MAX_BRIGHTNESS)
        *br = MAX_BRIGHTNESS;
    if (*br < MIN_BRIGHTNESS)
        *br = MIN_BRIGHTNESS;

    disp->setBrightness(*br);
}

void displayWriteNum(int16_t number, uint8_t width, uint8_t lead, uint8_t radix)
{
    uint8_t numdiv;
    uint8_t sign = lead;
    int8_t i;

    if (number < 0) {
        sign = '-';
        number = -number;
    }

    for (i = 0; i < width; i++)
        strbuf[i] = lead;
    strbuf[width] = '\0';
    i = width - 1;

    while (number > 0 || i == width - 1) {
        numdiv = number % radix;
        strbuf[i] = numdiv + 0x30;
        if (numdiv >= 10)
            strbuf[i] += 7;
        i--;
        number /= radix;
    }

    if (i >= 0)
        strbuf[i] = sign;

    if (disp->writeString) {
        disp->writeString(strbuf);
    }
}

void displayShowTime(RTC_type *rtc, char *wday)
{
    if (disp->showTime) {
        disp->showTime(rtc, wday);
    }
}

void displayShowParam(DispParam *dp)
{
    if (disp->showParam) {
        disp->showParam(dp);
    }
}

void displayShowSpectrum(uint8_t *dataL, uint8_t *dataR)
{
    if (disp->showSpectrum) {
        disp->showSpectrum(dataL, dataR);
    }
}
