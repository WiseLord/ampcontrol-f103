#include "canvas.h"

static const CanvasPalette canvasPalette = {
    .fg = LCD_COLOR_WHITE,
    .bg = LCD_COLOR_BLACK,
    .spCol = LCD_COLOR_ELECTRIC_BLUE,
    .spPeak = LCD_COLOR_WITCH_HAZE,
    .inactive = LCD_COLOR_NERO,
    .active = LCD_COLOR_AQUA,
};

static Canvas canvas = {
    .pal = &canvasPalette,
};

void canvasInit(Canvas **value)
{
    glcdInit(&canvas.glcd);
    *value = &canvas;
}

void canvasClear(void)
{
    glcdShift(0);
    glcdDrawRect(0, 0, canvas.glcd->drv->width, canvas.glcd->drv->height, canvas.pal->bg);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(canvas.pal->bg);
}
