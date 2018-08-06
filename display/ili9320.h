#ifndef ILI9320_H
#define ILI9320_H

#include <inttypes.h>

//LCD settings
#define ILI9320_WIDTH           240
#define ILI9320_HEIGHT          320
#define ILI9341_PIXEL           (ILI9320_WIDTH * ILI9320_HEIGHT)

//Colors
#define LCD_COLOR_WHITE         0xFFFF
#define LCD_COLOR_BLACK         0x0000
#define LCD_COLOR_RED           0xF800
#define LCD_COLOR_GREEN         0x07E0
#define LCD_COLOR_BLUE          0x001F
#define LCD_COLOR_YELLOW        0xFFE0
#define LCD_COLOR_CYAN          0x07FF
#define LCD_COLOR_MAGENTA       0xF81F

typedef enum {
    LCD_Orientation_Portrait_1,
    LCD_Orientation_Portrait_2,
    LCD_Orientation_Landscape_1,
    LCD_Orientation_Landscape_2
} LCD_Orientation;

typedef struct {
    uint16_t width;
    uint16_t height;
    LCD_Orientation orientation;
} LCD_Options;

void ILI9320_Init(void);

void ILI9320_Write(uint16_t data);
void ILI9320_SelectReg(uint16_t reg);
void ILI9320_WriteReg(uint16_t reg, uint16_t data);

void ILI9320_InitLCD(void);
void ILI9320_ExitSleep(void);
void ILI9320_EnterSleep(void);

void ILI9320_Rotate(LCD_Orientation orientation);

void ILI9320_SetCursor(uint16_t x, uint16_t y);
void ILI9320_SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

void ILI9320_Fill(uint16_t color);
void ILI9320_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

void ILI9320_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void ILI9320_DrawHorizLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color);
void ILI9320_DrawVertLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color);

void ILI9320_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void ILI9320_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                                 uint16_t color);
void ILI9320_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void ILI9320_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

#endif // ILI9320_H
