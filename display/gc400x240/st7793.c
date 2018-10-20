#include "st7793.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"

#define ST7793_WIDTH           240
#define ST7793_HEIGHT          400
#define ST7793_PIXELS          (ST7793_WIDTH * ST7793_HEIGHT)

static DispDriver drv = {
    .width = ST7793_HEIGHT,
    .height = ST7793_WIDTH,
    .drawPixel = st7793DrawPixel,
    .drawRectangle = st7793DrawRectangle,
    .drawImage = st7793DrawImage,
};

static inline void st7793SelectReg(uint16_t reg) __attribute__((always_inline));
static inline void st7793SelectReg(uint16_t reg)
{
    CLR(DISP_8BIT_RS);
    dispdrvSendData16(reg);
    SET(DISP_8BIT_RS);
}

static void st7793WriteReg(uint16_t reg, uint16_t data)
{
    st7793SelectReg(reg);
    dispdrvSendData16(data);
}

static inline void st7793InitSeq(void)
{
    // Wait for reset
    LL_mDelay(200);

    CLR(DISP_8BIT_CS);

    //-------------Display Control Setting-------------------------------------//

    st7793WriteReg(0x0001, 0x0100);
    st7793WriteReg(0x0003, 0x1020);
    st7793WriteReg(0x0008, 0x0808);
    st7793WriteReg(0x0090, 0x8000);
    st7793WriteReg(0x0400, 0x6200);
    st7793WriteReg(0x0401, 0x0001);

    //-------------End Display Control setting---------------------------------//
    //-------------Power Control Registers Initial ----------------------------//

    st7793WriteReg(0x00ff, 0x0001);
    st7793WriteReg(0x0102, 0x01b0);
    st7793WriteReg(0x0710, 0x0016);
    st7793WriteReg(0x0712, 0x000f);
    st7793WriteReg(0x0752, 0x002f);
    st7793WriteReg(0x0759, 0x0070);

    //--------------End Power Control Registers Initial ------------------------//
    LL_mDelay(100);
    //--------------Display Windows 240 X 400-----------------------------------//

    st7793WriteReg(0x0210, 0x0000);
    st7793WriteReg(0x0211, 0x00ef);
    st7793WriteReg(0x0212, 0x0000);
    st7793WriteReg(0x0213, 0x018f);

    //--------------End Display Windows 240 X 400-------------------------------//
    LL_mDelay(10);
    //--------------Gamma Cluster Setting---------------------------------------//

    st7793WriteReg(0x0380, 0x0100);
    st7793WriteReg(0x0381, 0x3718);
    st7793WriteReg(0x0382, 0x0802);
    st7793WriteReg(0x0383, 0x060f);
    st7793WriteReg(0x0384, 0x2223);
    st7793WriteReg(0x0385, 0x0000);
    st7793WriteReg(0x0386, 0x5816);
    st7793WriteReg(0x0387, 0x0503);
    st7793WriteReg(0x0388, 0x0510);
    st7793WriteReg(0x0389, 0x2233);

    //---------------End Gamma Setting------------------------------------------//
    //---------------Vcom Setting-----------------------------------------------//

    st7793WriteReg(0x0702, 0x0057);
    st7793WriteReg(0x00ff, 0x0000);

    //---------------End Vcom Setting-------------------------------------------//

    st7793WriteReg(0x0007, 0x0100);
    LL_mDelay(200);
    st7793WriteReg(0x0200, 0x0000);
    st7793WriteReg(0x0201, 0x0000);

    SET(DISP_8BIT_CS);
}

static inline void st7793SetWindow(uint16_t x, uint16_t y, uint16_t w,
                                   uint16_t h) __attribute__((always_inline));
static inline void st7793SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    st7793WriteReg(0x0210, ST7793_WIDTH - y - h);
    st7793WriteReg(0x0211, ST7793_WIDTH - y - 1);
    st7793WriteReg(0x0212, x);
    st7793WriteReg(0x0213, x + w - 1);

    // Set cursor
    st7793WriteReg(0x00200, ST7793_WIDTH - y - 1);
    st7793WriteReg(0x00201, x);

    // Select RAM mode
    st7793SelectReg(0x0202);
}

void st7793Init(DispDriver **driver)
{
    *driver = &drv;

    SET(DISP_8BIT_LED);
    SET(DISP_8BIT_RD);
    SET(DISP_8BIT_WR);
    SET(DISP_8BIT_RS);
    SET(DISP_8BIT_CS);

    CLR(DISP_8BIT_RST);
    LL_mDelay(1);
    SET(DISP_8BIT_RST);

    st7793InitSeq();
}

void st7793Sleep(void)
{
    CLR(DISP_8BIT_CS);

    st7793WriteReg(0x0007, 0x0000);
    LL_mDelay(50);
    st7793WriteReg(0x0102, 0x0180);
    LL_mDelay(200);

    SET(DISP_8BIT_CS);
}

void st7793Wakeup(void)
{
    CLR(DISP_8BIT_CS);

    // Power On Sequence
    LL_mDelay(200);
    st7793WriteReg(0x0102, 0x01b0);
    LL_mDelay(50);
    st7793WriteReg(0x0007, 0x0100);

    SET(DISP_8BIT_CS);
}

void st7793DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_8BIT_CS);

    st7793SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_8BIT_CS);
}

void st7793DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(DISP_8BIT_CS);

    st7793SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_8BIT_CS);
}

void st7793DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    uint16_t w = img->width;
    uint16_t h = img->height;

    CLR(DISP_8BIT_CS);

    st7793SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_8BIT_CS);
}
