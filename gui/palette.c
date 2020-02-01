#include "palette.h"

static PalIdx palIdx = PAL_AQUA;

static const Palette palette[PAL_END] = {
    [PAL_SNOW] = {
        .fg = COLOR_WHITE,
        .bg = COLOR_BLACK,
        .active = COLOR_AQUA,
        .inactive = COLOR_NERO,
        .selected = COLOR_ECLIPSE,
        .spColB = RGB_CONV(0x00FFFF),
        .spColG = RGB_CONV(0xFFFFFF),
        .spPeak = RGB_CONV(0x00FFFF),
    },
    [PAL_AQUA] = {
        .fg = RGB_CONV(0xB0FFFF),
        .bg = COLOR_BLACK,
        .active = COLOR_AQUA,
        .inactive = COLOR_NERO,
        .selected = COLOR_ECLIPSE,
        .spColB = RGB_CONV(0x40FFB0),
        .spColG = RGB_CONV(0xB0FFFF),
        .spPeak = RGB_CONV(0xFFFF80),
    },
    [PAL_FIRE] = {
        .fg = RGB_CONV(0xFFFFC0),
        .bg = COLOR_BLACK,
        .active = COLOR_AQUA,
        .inactive = COLOR_NERO,
        .selected = COLOR_ECLIPSE,
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
