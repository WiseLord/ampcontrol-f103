#include "palette.h"

static PalIdx palIdx = PAL_AQUA;

static const Palette palette[PAL_END] = {
    [PAL_SNOW] = {
        .fg = LCD_COLOR_WHITE,
        .bg = LCD_COLOR_BLACK,
        .active = LCD_COLOR_AQUA,
        .inactive = LCD_COLOR_NERO,
        .selected = LCD_COLOR_ECLIPSE,
        .spColB = RGB_CONV(0x00FFFF),
        .spColG = RGB_CONV(0xFFFFFF),
        .spPeak = RGB_CONV(0x00FFFF),
    },
    [PAL_AQUA] = {
        .fg = RGB_CONV(0xB0FFFF),
        .bg = LCD_COLOR_BLACK,
        .active = LCD_COLOR_AQUA,
        .inactive = LCD_COLOR_NERO,
        .selected = LCD_COLOR_ECLIPSE,
        .spColB = RGB_CONV(0x40FFB0),
        .spColG = RGB_CONV(0xB0FFFF),
        .spPeak = RGB_CONV(0xFFFF80),
    },
    [PAL_FIRE] = {
        .fg = RGB_CONV(0xFFFFC0),
        .bg = LCD_COLOR_BLACK,
        .active = LCD_COLOR_AQUA,
        .inactive = LCD_COLOR_NERO,
        .selected = LCD_COLOR_ECLIPSE,
        .spColB = RGB_CONV(0xFFFF00),
        .spColG = RGB_CONV(0xFF0000),
        .spPeak = RGB_CONV(0xFF8000),
    },
};

const Palette *paletteGet(PalIdx index)
{
    return &palette[index];
}

PalIdx paletteGetIndex(void)
{
    return palIdx;
}

void paletteSetIndex(PalIdx index)
{
    palIdx = index;
}
