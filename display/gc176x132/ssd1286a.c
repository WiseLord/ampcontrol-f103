#include "ssd1286a.h"

#include "../dispcanvas.h"
#include "../dispdrv.h"

#include "../../pins.h"
#include "../../functions.h"

#define SSD1286A_WIDTH           132
#define SSD1286A_HEIGHT          176
#define SSD1286A_PIXELS          (SSD1286A_WIDTH * SSD1286A_HEIGHT)

static GlcdDriver glcd = {
    .clear = ssd1286aClear,
    .drawPixel = ssd1286aDrawPixel,
    .drawRectangle = ssd1286aDrawRectangle,
    .drawImage = ssd1286aDrawImage,
};

static inline void ssd1286aSendCmd(uint8_t cmd) __attribute__((always_inline));
static inline void ssd1286aSendCmd(uint8_t cmd)
{
    dispdrvWaitOperation();
    CLR(DISP_SPI_DC);

    dispdrvSendData8(cmd);

    dispdrvWaitOperation();
    SET(DISP_SPI_DC);
}

static void ssd1286aInitSeq(void)
{
    _delay_ms(50);

    CLR(DISP_SPI_CS);

    ssd1286aSendCmd(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);

    ssd1286aSendCmd(0x10);
    dispdrvSendData8(0x1f);
    dispdrvSendData8(0x92);

    ssd1286aSendCmd(0x11);
    dispdrvSendData8(0x61);
    dispdrvSendData8(0x1c);

    ssd1286aSendCmd(0x28);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x06);

    ssd1286aSendCmd(0x02);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ssd1286aSendCmd(0x12);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x0f);

    ssd1286aSendCmd(0x01);
    dispdrvSendData8(0x30);
    dispdrvSendData8(0xb0);

    ssd1286aSendCmd(0x03);
    dispdrvSendData8(0x78);
    dispdrvSendData8(0x30);

    ssd1286aSendCmd(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x33);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}


void ssd1286aSetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ssd1286aSendCmd(0x44);
    dispdrvSendData8(y + h - 1);
    dispdrvSendData8(y);

    ssd1286aSendCmd(0x47);
    dispdrvSendData8(x + w - 1);
    dispdrvSendData8(x);

    ssd1286aSendCmd(0x21);
    dispdrvSendData8(x);
    dispdrvSendData8(y);

    ssd1286aSendCmd(0x22);
}

void ssd1286aInit(GlcdDriver **driver)
{
    *driver = &glcd;
    gc176x132Init(*driver);

    CLR(DISP_SPI_RST);
    _delay_ms(100);
    SET(DISP_SPI_RST);

    // Init magic
    ssd1286aInitSeq();
    SET(DISP_SPI_LED);
}

void ssd1286aClear(void)
{
    ssd1286aDrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, glcd.canvas->color);
}

void ssd1286aSleep(void)
{
    CLR(DISP_SPI_CS);

    ssd1286aSendCmd(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ssd1286aSendCmd(0x10);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);

    ssd1286aSendCmd(0x11);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ssd1286aSendCmd(0x12);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ssd1286aWakeup(void)
{
    CLR(DISP_SPI_CS);

    ssd1286aSendCmd(0x10);
    dispdrvSendData8(0x1f);
    dispdrvSendData8(0x92);

    _delay_ms(20);

    ssd1286aSendCmd(0x11);
    dispdrvSendData8(0x61);
    dispdrvSendData8(0x1c);

    _delay_ms(20);

    ssd1286aSendCmd(0x12);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x0f);

    _delay_ms(20);

    ssd1286aSendCmd(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x33);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ssd1286aDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_SPI_CS);

    ssd1286aSetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ssd1286aDrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(DISP_SPI_CS);

    ssd1286aSetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ssd1286aDrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;

    CLR(DISP_SPI_CS);

    ssd1286aSetWindow(x0, y0, w, h);
    dispdrvSendImage(img, w, h);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}
