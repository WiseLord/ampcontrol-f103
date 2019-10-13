#include "../dispdrv.h"

void st7793Init(void)
{
    CLR(DISP_CS);

    //-------------Display Control Setting-------------------------------------//

    dispdrvWriteReg16(0x0001, 0x0100);
    dispdrvWriteReg16(0x0003, 0x1030);
    dispdrvWriteReg16(0x0008, 0x0808);
    dispdrvWriteReg16(0x0090, 0x8000);
    dispdrvWriteReg16(0x0400, 0xE200);
    dispdrvWriteReg16(0x0401, 0x0003);

    //-------------End Display Control setting---------------------------------//
    //-------------Power Control Registers Initial ----------------------------//

    dispdrvWriteReg16(0x00ff, 0x0001);
    dispdrvWriteReg16(0x0102, 0x01b0);
    dispdrvWriteReg16(0x0710, 0x0016);
    dispdrvWriteReg16(0x0712, 0x000f);
    dispdrvWriteReg16(0x0752, 0x002f);
    dispdrvWriteReg16(0x0759, 0x0070);

    //--------------End Power Control Registers Initial ------------------------//
    utilmDelay(100);
    //--------------Display Windows 240 X 400-----------------------------------//

    dispdrvWriteReg16(0x0210, 0x0000);
    dispdrvWriteReg16(0x0211, 0x00ef);
    dispdrvWriteReg16(0x0212, 0x0000);
    dispdrvWriteReg16(0x0213, 0x018f);

    //--------------End Display Windows 240 X 400-------------------------------//
    utilmDelay(10);
    //--------------Gamma Cluster Setting---------------------------------------//

    dispdrvWriteReg16(0x0380, 0x0100);
    dispdrvWriteReg16(0x0381, 0x3718);
    dispdrvWriteReg16(0x0382, 0x0802);
    dispdrvWriteReg16(0x0383, 0x060f);
    dispdrvWriteReg16(0x0384, 0x2223);
    dispdrvWriteReg16(0x0385, 0x0000);
    dispdrvWriteReg16(0x0386, 0x5816);
    dispdrvWriteReg16(0x0387, 0x0503);
    dispdrvWriteReg16(0x0388, 0x0510);
    dispdrvWriteReg16(0x0389, 0x2233);

    //---------------End Gamma Setting------------------------------------------//
    //---------------Vcom Setting-----------------------------------------------//

    dispdrvWriteReg16(0x0702, 0x0057);
    dispdrvWriteReg16(0x00ff, 0x0000);

    //---------------End Vcom Setting-------------------------------------------//

    dispdrvWriteReg16(0x0007, 0x0100);
    utilmDelay(200);
    dispdrvWriteReg16(0x0200, 0x0000);
    dispdrvWriteReg16(0x0201, 0x0000);

    SET(DISP_CS);
}

void st7793Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate) {
        dispdrvWriteReg16(0x0001, 0x0000);
        dispdrvWriteReg16(0x0400, 0x6200);
    } else {
        dispdrvWriteReg16(0x0001, 0x0100);
        dispdrvWriteReg16(0x0400, 0xE200);
    }

    SET(DISP_CS);
}

void st7793Shift(int16_t value)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0404, (uint16_t)value);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void st7793Sleep(void)
{
    CLR(DISP_CS);

    dispdrvWriteReg16(0x0007, 0x0000);
    utilmDelay(50);
    dispdrvWriteReg16(0x0102, 0x0180);
    utilmDelay(200);

    SET(DISP_CS);
}

void st7793Wakeup(void)
{
    CLR(DISP_CS);

    // Power On Sequence
    utilmDelay(200);
    dispdrvWriteReg16(0x0102, 0x01b0);
    utilmDelay(50);
    dispdrvWriteReg16(0x0007, 0x0100);

    SET(DISP_CS);
}

void st7793SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvWriteReg16(0x0210, (uint16_t)y);
    dispdrvWriteReg16(0x0211, (uint16_t)y1);
    dispdrvWriteReg16(0x0212, (uint16_t)x);
    dispdrvWriteReg16(0x0213, (uint16_t)x1);

    // Set cursor
    dispdrvWriteReg16(0x00200, (uint16_t)y);
    dispdrvWriteReg16(0x00201, (uint16_t)x);

    // Select RAM mode
    dispdrvSelectReg16(0x0202);
}

const DispDriver dispdrv = {
    .width = 400,
    .height = 240,
    .init = st7793Init,
    .sleep = st7793Sleep,
    .wakeup = st7793Wakeup,
    .setWindow = st7793SetWindow,
    .rotate = st7793Rotate,
//    .shift = st7793Shift,
};
