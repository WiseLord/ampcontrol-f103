#include "canvas.h"

static Canvas canvas;

void canvasInit(Canvas **value)
{
    canvas.color = LCD_COLOR_BLACK;
    glcdInit(&canvas.glcd);
    *value = &canvas;
}

void canvasClear(void)
{
    glcdShift(0);
    glcdDrawRect(0, 0, canvas.glcd->drv->width, canvas.glcd->drv->height, canvas.color);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(canvas.color);
}
