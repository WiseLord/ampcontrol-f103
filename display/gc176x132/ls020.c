#include "ls020.h"

#include "../dispdrv.h"
#include "../../pins.h"
#include "../../functions.h"

#define LS020_WIDTH           132
#define LS020_HEIGHT          176
#define LS020_PIXELS          (LS020_WIDTH * LS020_HEIGHT)

static GlcdDriver glcd = {
    .clear = ls020Clear,
    .drawPixel = ls020DrawPixel,
    .drawRectangle = ls020DrawRectangle,
    .drawImage = ls020DrawImage,
};
/*
static inline void ls020SendSPI(uint8_t data) __attribute__((always_inline));
static inline void ls020SendSPI(uint8_t data)
{
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData8(SPI1, data);
}

static inline void dispdrvSendData16(uint16_t data) __attribute__((always_inline));
static inline void dispdrvSendData16(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    ls020SendSPI(dataH);
    ls020SendSPI(dataL);
}
*/
static void ls020InitSeq(void)
{
    _delay_ms(50);
    SET(DISP_SPI_DC);
    CLR(DISP_SPI_CS);

    dispdrvSendData16(0xFDFD);
    dispdrvSendData16(0xFDFD);
    _delay_ms(50);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0xEE04);
    dispdrvSendData16(0x1B04);
    dispdrvSendData16(0xFEFE);
    dispdrvSendData16(0xFEFE);
    dispdrvSendData16(0xEF90);
    dispdrvSendData16(0x4A04);
    dispdrvSendData16(0x7F3F);
    dispdrvSendData16(0xEE04);
    dispdrvSendData16(0x4306);
    _delay_ms(50);
    dispdrvSendData16(0xEF90);
    dispdrvSendData16(0x0983);
    dispdrvSendData16(0x0800);
    dispdrvSendData16(0x0BAF);
    dispdrvSendData16(0x0A00);
    dispdrvSendData16(0x0500);
    dispdrvSendData16(0x0600);
    dispdrvSendData16(0x0700);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0xEE0C);
    dispdrvSendData16(0xEF90);
    dispdrvSendData16(0x0080);
    dispdrvSendData16(0xEFB0);
    dispdrvSendData16(0x4902);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7F01);
    dispdrvSendData16(0xE181);
    dispdrvSendData16(0xE202);
    dispdrvSendData16(0xE276);
    dispdrvSendData16(0xE183);
    _delay_ms(50);
    dispdrvSendData16(0x8001);
    dispdrvSendData16(0xEF90);
    dispdrvSendData16(0x0020);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}


void ls020SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    SET(DISP_SPI_DC);
    CLR(DISP_SPI_CS);

    dispdrvSendData16(0x0800 + LS020_WIDTH - y - h);
    dispdrvSendData16(0x0900 + LS020_WIDTH - y - 1);


    dispdrvSendData16(0x0A00 + x);
    dispdrvSendData16(0x0B00 + x + w - 1);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ls020Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc176x132Init(*driver);

    CLR(DISP_SPI_RST);
    _delay_ms(100);
    SET(DISP_SPI_RST);

    // Init magic
    ls020InitSeq();
    SET(DISP_SPI_LED);
}

void ls020Clear(void)
{
    ls020DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, glcd.canvas->color);
}

void ls020Sleep(void)
{
    SET(DISP_SPI_DC);
    CLR(DISP_SPI_CS);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7E04);
    dispdrvSendData16(0xEFB0);
    dispdrvSendData16(0x5A48);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7F01);
    dispdrvSendData16(0xEFB0);
    dispdrvSendData16(0x64FF);
    dispdrvSendData16(0x6500);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7F01);
    dispdrvSendData16(0xE262);
    dispdrvSendData16(0xE202);
    dispdrvSendData16(0xEFB0);
    dispdrvSendData16(0xBC02);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7F01);
    dispdrvSendData16(0xE200);
    dispdrvSendData16(0x8000);
    dispdrvSendData16(0xE204);
    dispdrvSendData16(0xE200);
    dispdrvSendData16(0xE100);
    dispdrvSendData16(0xEFB0);
    dispdrvSendData16(0xBC00);
    dispdrvSendData16(0xEF00);
    dispdrvSendData16(0x7F01);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ls020Wakeup(void)
{
    ls020InitSeq();
}

void ls020DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    ls020SetWindow(x, y, 1, 1);

    CLR(DISP_SPI_DC);
    CLR(DISP_SPI_CS);
    dispdrvSendData16(color);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ls020DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    ls020SetWindow(x, y, w, h);

    CLR(DISP_SPI_DC);
    CLR(DISP_SPI_CS);

    dispdrvSendFill(w * h, color);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ls020DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;

    ls020SetWindow(x0, y0, w, h);

    CLR(DISP_SPI_DC);
    CLR(DISP_SPI_CS);

    dispdrvSendImage(img, w, h);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}
