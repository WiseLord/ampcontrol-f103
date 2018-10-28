#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include "../dispdrv.h"

#define SSD1306_I2C_ADDR_0              0x78
#define SSD1306_I2C_ADDR_1              0x7A

#define SSD1306_I2C_COMMAND             0x00
#define SSD1306_I2C_DATA_SEQ            0x40

#define SSD1306_USE_PAGE_ADDRESSING
#define SSD1306_ROTATE_180

#define SSD1306_MIN_BRIGHTNESS          0
#define SSD1306_MAX_BRIGHTNESS          32

void ssd1306Init(DispDriver **driver);
void ssd1306Clear();

void ssd1306DrawPixel(int16_t x, int16_t y, uint16_t color);

void ssd1306SetBrightness(uint8_t br);
void ssd1306UpdateFb();

#endif // SSD1306_H
