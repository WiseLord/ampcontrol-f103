#include "palette.h"

#include "../display/glcd.h"

static PalIdx palIdx = PAL_AQUA;

static const Palette palette[PAL_END] = {
    [PAL_SNOW] = {
        .fg = LCD_COLOR_WHITE,
        .bg = LCD_COLOR_BLACK,
        .active = LCD_COLOR_AQUA,
        .inactive = LCD_COLOR_NERO,
        .selected = LCD_COLOR_ECLIPSE,
        .spCol = RGB_TO_565(0xB0FFFF),
        .spPeak = LCD_COLOR_WHITE,
    },
    [PAL_AQUA] = {
        .fg = RGB_TO_565(0xB0FFFF),
        .bg = LCD_COLOR_BLACK,
        .active = LCD_COLOR_AQUA,
        .inactive = LCD_COLOR_NERO,
        .selected = LCD_COLOR_ECLIPSE,
        .spCol = LCD_COLOR_ELECTRIC_BLUE,
        .spPeak = LCD_COLOR_WITCH_HAZE,
    },
    [PAL_FIRE] = {
        .fg = RGB_TO_565(0xFFFFC0),
        .bg = LCD_COLOR_BLACK,
        .active = LCD_COLOR_AQUA,
        .inactive = LCD_COLOR_NERO,
        .selected = LCD_COLOR_ECLIPSE,
        .spCol = RGB_TO_565(0xFF8040),
        .spPeak = LCD_COLOR_DARK_ORANGE,
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
