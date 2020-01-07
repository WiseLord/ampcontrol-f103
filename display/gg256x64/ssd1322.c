#include "ssd1322.h"

#include "../dispdrv.h"

#define FB_SIZE     (DISP_WIDTH * DISP_HEIGHT / 2)

static uint8_t fb[FB_SIZE];

void ssd1322Init(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(SSD1322_SET_COMMAND_LOCK);
    dispdrvSendData8(0x12);// Unlock (reset)

    dispdrvSelectReg8(SSD1322_SET_DISPLAY_OFF);

    dispdrvSelectReg8(SSD1322_SET_CLOCK_DIVIDER);// 0xB3
    dispdrvSendData8(0x91);

    dispdrvSelectReg8(SSD1322_SET_MUX_RATIO);
    dispdrvSendData8(0x3F); // 64MUX (reset 0x7F => 128MUX)

    dispdrvSelectReg8(SSD1322_SET_DISPLAY_OFFSET);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(SSD1322_SET_DISPLAY_START_LINE);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(SSD1322_SET_REMAP_AND_DUAL_COM_LINE_MODE);
    dispdrvSendData8(0x06); // 0x14 to rotate 180
    dispdrvSendData8(0x11);

    dispdrvSelectReg8(SSD1322_SET_GPIO);
    dispdrvSendData8(0x00); // Disabled both GPIO1/GPIO2

    dispdrvSelectReg8(SSD1322_FUNCTION_SELECTION);
    dispdrvSendData8(0x01); // Internal Vdd regulator

    dispdrvSelectReg8(SSD1322_ENABLE_VSL_EXTERN);
    dispdrvSendData8(0xA0); // Enable the external VSL
    dispdrvSendData8(0xFD); // Enhanced low GS display quality; (reset 0xB5 (normal)),

    dispdrvSelectReg8(SSD1322_SET_CONTRAST_CURRENT);
    dispdrvSendData8(0xFF); // (reset 0x7F)

    dispdrvSelectReg8(SSD1322_MASTER_CURRENT_CONTROL);
    dispdrvSendData8(0x0F); // (reset)

    dispdrvSelectReg8(SSD1322_SET_GRAY_SCALE_TABLE);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x06);
    dispdrvSendData8(0x0A);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x17);
    dispdrvSendData8(0x21);
    dispdrvSendData8(0x2c);
    dispdrvSendData8(0x3b);
    dispdrvSendData8(0x4c);
    dispdrvSendData8(0x60);
    dispdrvSendData8(0x78);
    dispdrvSendData8(0x94);
    dispdrvSendData8(0xB4);
    dispdrvSelectReg8(SSD1322_ENABLE_GRAY_SCALE_TABLE);

    dispdrvSelectReg8(SSD1322_SET_PHASE_LENGTH);
    dispdrvSendData8(0xE8); // 14 DCLKs phase 1, 17 DCLKs phase 2 (reset 0x74 => 7 + 9 DCLKs)

    dispdrvSelectReg8(SSD1322_ENHANCE_DRIVING_SCHEME_CAPABILITY);// 0xD1
    dispdrvSendData8(0x82); // Reserved; default is 0xA2 (normal)
    dispdrvSendData8(0x20);

    dispdrvSelectReg8(SSD1322_SET_PRECHARGE_VOLTAGE);
    dispdrvSendData8(0x1F); // 0.6 Vcc (reset 0x17)

    dispdrvSelectReg8(SSD1322_SET_SECOND_PRECHARGE_PERIOD);
    dispdrvSendData8(0x08); // 8 DCLKs (reset)

    dispdrvSelectReg8(SSD1322_SET_VCOMH);
    dispdrvSendData8(0x07); // 0.86 Vcc (reset 0x04 => 0.86 Vcc)

    dispdrvSelectReg8(SSD1322_EXIT_PARTIAL_DISPLAY);

    dispdrvSelectReg8(SSD1322_NORMAL_DISPLAY);

    dispdrvSelectReg8(SSD1322_SET_DISPLAY_ON);

    SET(DISP_CS);
}

void ssd1322Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(SSD1322_SET_REMAP_AND_DUAL_COM_LINE_MODE);
    dispdrvSendData8(rotate & LCD_ROTATE_180 ? 0x14 : 0x06);
    dispdrvSendData8(0x11);

    dispdrvSelectReg8(SSD1322_SET_PHASE_LENGTH);
    dispdrvSendData8(0xE8); // 14 DCLKs phase 1, 17 DCLKs phase 2 (reset 0x74 => 7 + 9 DCLKs)

    SET(DISP_CS);
}

void ssd1322FbSync(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(SSD1322_SET_COLUMN_ADDRESS);
    dispdrvSendData8(MIN_SEG);
    dispdrvSendData8(MAX_SEG);

    dispdrvSelectReg8(SSD1322_SET_ROW_ADDRESS);
    dispdrvSendData8(0);
    dispdrvSendData8(63);

    dispdrvSelectReg8(SSD1322_WRITE_RAM_COMMAND);

    for (int32_t i = 0; i < FB_SIZE; i++) {
        dispdrvSendData8(fb[i]);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1322FbSetPixel(int16_t x, int16_t y, color_t color)
{
    if (x % 2 == 0) {
        fb[128 * y + x / 2] &= 0x0F;
        fb[128 * y + x / 2] |= ((color << 4) & 0xF0);
    } else {
        fb[128 * y + x / 2] &= 0xF0;
        fb[128 * y + x / 2] |= ((color) & 0x0F);
    }
}

const DispDriver dispdrv = {
    .width = DISP_WIDTH,
    .height = DISP_HEIGHT,
    .init = ssd1322Init,
//    .sleep = ssd1322Sleep,
//    .wakeup = ssd1322Wakeup,
    .rotate = ssd1322Rotate,
//    .shift = ssd1322Shift,

    .fb = fb,
    .fbSync = ssd1322FbSync,
    .fbSetPixel = ssd1322FbSetPixel,
};
