#include "st7793.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 400,
    .height = 240,
    .drawPixel = st7793DrawPixel,
    .drawRectangle = st7793DrawRectangle,
    .drawImage = st7793DrawImage,
    .rotate = st7793Rotate,
};

__attribute__((always_inline))
static inline void st7793SelectReg(uint16_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);
}

__attribute__((always_inline))
static inline void st7793WriteReg(uint16_t reg, uint16_t data)
{
    st7793SelectReg(reg);
    dispdrvSendData16(data);
}

__attribute__((always_inline))
static inline void st7793SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    st7793WriteReg(0x0210, (uint16_t)y);
    st7793WriteReg(0x0211, (uint16_t)y1);
    st7793WriteReg(0x0212, (uint16_t)x);
    st7793WriteReg(0x0213, (uint16_t)x1);

    // Set cursor
    st7793WriteReg(0x00200, (uint16_t)y);
    st7793WriteReg(0x00201, (uint16_t)x);

    // Select RAM mode
    st7793SelectReg(0x0202);
}

static inline void st7793InitSeq(void)
{
    CLR(DISP_CS);

    //-------------Display Control Setting-------------------------------------//

    st7793WriteReg(0x0001, 0x0100);
    st7793WriteReg(0x0003, 0x1030);
    st7793WriteReg(0x0008, 0x0808);
    st7793WriteReg(0x0090, 0x8000);
    st7793WriteReg(0x0400, 0xE200);
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

    SET(DISP_CS);
}

void st7793Init(DispDriver **driver)
{
    *driver = &drv;
    st7793InitSeq();
}

void st7793Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate) {
        st7793WriteReg(0x0001, 0x0000);
        st7793WriteReg(0x0400, 0x6200);
    } else {
        st7793WriteReg(0x0001, 0x0100);
        st7793WriteReg(0x0400, 0xE200);
    }

    SET(DISP_CS);
}

void st7793Sleep(void)
{
    CLR(DISP_CS);

    st7793WriteReg(0x0007, 0x0000);
    LL_mDelay(50);
    st7793WriteReg(0x0102, 0x0180);
    LL_mDelay(200);

    SET(DISP_CS);
}

void st7793Wakeup(void)
{
    CLR(DISP_CS);

    // Power On Sequence
    LL_mDelay(200);
    st7793WriteReg(0x0102, 0x01b0);
    LL_mDelay(50);
    st7793WriteReg(0x0007, 0x0100);

    SET(DISP_CS);
}

void st7793DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    st7793SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void st7793DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    st7793SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void st7793DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    st7793SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}
