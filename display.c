#include "display.h"

#include "display/gdfb.h"
#include "actions.h"

static int8_t brStby;
static int8_t brWork;

static char strbuf[STR_BUFSIZE + 1];   // String buffer

static void displayWriteString(char *string)
{
    gdWriteString(string);
}

static void displayWriteNum(int16_t number, uint8_t width, uint8_t lead, uint8_t radix)
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

    displayWriteString(strbuf);
}

static void displayShowBar(int16_t min, int16_t max, int16_t value)
{
    uint8_t i, j;
    uint8_t color;

    if (min + max) {
        value = (uint16_t)91 * (value - min) / (max - min);
    } else {
        value = (int16_t)45 * value / max;
    }

    for (i = 0; i < 91; i++) {
        if (((min + max) && (value <= i)) || (!(min + max) &&
                                              (((value > 0) && ((i < 45) || (value + 45 < i))) ||
                                               ((value <= 0) && ((i > 45) || (value + 45 > i)))))) {
            color = 0x00;
        } else {
            color = 0x01;
        }
        if (!(i & 0x01)) {
            for (j = 27; j < 38; j++) {
                if (j == 32) {
                    gdDrawPixel(i, j, 1);
                } else {
                    gdDrawPixel(i, j, color);
                }
            }
        }
    }
}

static void displayShowIcon(uint8_t icon)
{
    if (icon < ICON24_END) {
        gdSetXY(104, 2);
        gdWriteIcon24(icon);
    }
}

void displayInit()
{
    // TODO: Read from backup memory
    brStby = GD_MAX_BRIGHTNESS / 8;
    brWork = GD_MAX_BRIGHTNESS;
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

    gdSetBrightness(*br);
}

void displayShowParam(DispParam *dp)
{
    gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
    gdSetXY(0, 0);
    gdWriteString((char *)dp->label);

    displayShowBar(dp->min, dp->max, dp->value);

    gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
    gdSetXY(94, 30);
    displayWriteNum(dp->value, 3, ' ', 10);

    displayShowIcon(dp->icon);
}