#include "gc320x240.h"

#include "ili9320.h"

void gc320x240Init(Display **disp)
{
    ILI9320_Init();
    // Rotage 90
    ILI9320_Rotate(LCD_Orientation_Landscape_1);

    ILI9320_DrawFilledCircle(160, 120, 59, LCD_COLOR_BLACK);
    ILI9320_DrawFilledCircle(160, 120, 56, LCD_COLOR_WHITE);
    ILI9320_DrawFilledCircle(160, 120, 53, LCD_COLOR_BLACK);
}
