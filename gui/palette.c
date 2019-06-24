#include "palette.h"

#include "../display/glcd.h"

static const Palette palBase = {
    .fg = LCD_COLOR_WHITE,
    .bg = LCD_COLOR_BLACK,
    .active = LCD_COLOR_AQUA,
    .inactive = LCD_COLOR_NERO,
    .selected = LCD_COLOR_ECLIPSE,
    .spCol = LCD_COLOR_ELECTRIC_BLUE,
    .spPeak = LCD_COLOR_WITCH_HAZE,
};

const Palette *paletteGet(void)
{
    return &palBase;
}
