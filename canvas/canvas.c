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

void canvasClear(ClearRegion clReg)
{
    glcdShift(0);

    int16_t x = 0;
    int16_t y = 0;
    int16_t w = canvas.glcd->drv->width;
    int16_t h = canvas.glcd->drv->height;

    switch (clReg) {
    case CLEAR_CENTER:
        x = w / 8;
        y = h / 6;
        w = x * 6;
        h = y * 4;
        glcdDrawFrame(x - 2, y - 2, w + 4, h + 4, 2, LCD_COLOR_WHITE);
        break;
    default:
        break;
    }
    glcdDrawRect(x, y, w, h, canvas.pal->bg);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(canvas.pal->bg);
}
