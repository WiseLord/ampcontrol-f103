#include "ili9341.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_spi.h>

#include "../pins.h"
#include "../functions.h"

#include "gc320x240.h"

#define ILI9341_WIDTH           240
#define ILI9341_HEIGHT          320
#define ILI9341_PIXELS          (ILI9341_WIDTH * ILI9341_HEIGHT)

DisplayDriver drv = {
    .clear = ili9341Clear,
    .setBrightness = glcdSetBrightness,
    .drawPixel = ili9341DrawPixel,
    .drawRectangle = ili9341DrawRectangle,
    .drawFontChar = glcdDrawFontChar,
//    .drawFontChar = ili9341DrawFontChar,
};

static void ili9341SendSPI(uint8_t data)
{
    uint8_t i;

    for (i = 0; i < 8; i++) {
        if (data & 0x80)
            SET(ILI9341_SDI);
        else
            CLR(ILI9341_SDI);

        SET(ILI9341_SCK);
        CLR(ILI9341_SCK);

        data <<= 1;
    }
    _delay_us(1);
//    while (LL_SPI_IsActiveFlag_TXE(SPI1));
//    LL_SPI_TransmitData8(SPI1, data);
//    while (!(SPSR & (1 << SPIF)));
//    SPDR = data;
}

static void ili9341WriteData(uint8_t data)
{
    CLR(ILI9341_CS);
    ili9341SendSPI(data);
    _delay_us(1);
//    LL_SPI_TransmitData8(SPI1, data);
//    while (LL_SPI_IsActiveFlag_TXE(SPI1));
//    SPDR = data;
//    while (!(SPSR & (1 << SPIF)));  // Wait for transmittion to complete
    SET(ILI9341_CS);
}

static void ili9341SendCmd(uint8_t cmd)
{
    CLR(ILI9341_DC);
    CLR(ILI9341_CS);
    ili9341SendSPI(cmd);
    _delay_us(1);
//    LL_SPI_TransmitData8(SPI1, cmd);
//    while (LL_SPI_IsActiveFlag_TXE(SPI1));
//    SPDR = cmd;
//    while (!(SPSR & (1 << SPIF)));  // Wait for transmittion to complete
    SET(ILI9341_CS);
    SET(ILI9341_DC);
}

static void ili9341SendData(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;

    CLR(ILI9341_CS);
    ili9341SendSPI(dataH);
    ili9341SendSPI(dataL);
    _delay_us(1);
//    while (LL_SPI_IsActiveFlag_TXE(SPI1));
//    while (!(SPSR & (1 << SPIF)));  // Wait for last transmission to complete
    SET(ILI9341_CS);
}

static void ili9341InitSeq(void)
{
    _delay_ms(50);

    ili9341SendCmd(ILI9341_SWRESET);
    _delay_ms(10);
    ili9341SendCmd(ILI9341_DISPOFF);

    ili9341SendCmd(ILI9341_PWCTRLB);
    ili9341WriteData(0x00);
    ili9341WriteData(0xC1);
    ili9341WriteData(0x30);

    ili9341SendCmd(ILI9341_PWSEQCTL);
    ili9341WriteData(0x64);
    ili9341WriteData(0x03);
    ili9341WriteData(0x12);
    ili9341WriteData(0x81);

    ili9341SendCmd(ILI9341_DRVTIMCTLA1);
    ili9341WriteData(0x85);
    ili9341WriteData(0x01);
    ili9341WriteData(0x79);

    ili9341SendCmd(ILI9341_PWCTRLA);
    ili9341WriteData(0x39);
    ili9341WriteData(0x2C);
    ili9341WriteData(0x00);
    ili9341WriteData(0x34);
    ili9341WriteData(0x02);

    ili9341SendCmd(ILI9341_PUMPRTCTL);
    ili9341WriteData(0x20);

    ili9341SendCmd(ILI9341_DRVTIMB);
    ili9341WriteData(0x00);
    ili9341WriteData(0x00);

    ili9341SendCmd(ILI9341_PWCTRL1);
    ili9341WriteData(0x26);

    ili9341SendCmd(ILI9341_PWCTRL2);
    ili9341WriteData(0x11);

    ili9341SendCmd(ILI9341_VMCTRL1);
    ili9341WriteData(0x35);
    ili9341WriteData(0x3E);

    ili9341SendCmd(ILI9341_VMCTRL2);
    ili9341WriteData(0xBE);

    ili9341SendCmd(ILI9341_PIXSET);
    ili9341WriteData(0x55);

    ili9341SendCmd(ILI9341_FRMCTR1);
    ili9341WriteData(0x00);
    ili9341WriteData(0x1B);

    ili9341SendCmd(ILI9341_EN3G);
    ili9341WriteData(0x08);

    ili9341SendCmd(ILI9341_GAMSET);
    ili9341WriteData(0x01);

    ili9341SendCmd(ILI9341_ETMOD);
    ili9341WriteData(0x07);

    ili9341SendCmd(ILI9341_DISCTRL);
    ili9341WriteData(0x08);
    ili9341WriteData(0x82);
    ili9341WriteData(0x27);

    ili9341SendCmd(ILI9341_PGAMCTRL);
    ili9341WriteData(0x0F);
    ili9341WriteData(0x31);
    ili9341WriteData(0x2B);
    ili9341WriteData(0x0C);
    ili9341WriteData(0x0E);
    ili9341WriteData(0x08);
    ili9341WriteData(0x4E);
    ili9341WriteData(0xF1);
    ili9341WriteData(0x37);
    ili9341WriteData(0x07);
    ili9341WriteData(0x10);
    ili9341WriteData(0x03);
    ili9341WriteData(0x0E);
    ili9341WriteData(0x09);
    ili9341WriteData(0x00);

    ili9341SendCmd(ILI9341_NGAMCTRL);
    ili9341WriteData(0x00);
    ili9341WriteData(0x0E);
    ili9341WriteData(0x14);
    ili9341WriteData(0x03);
    ili9341WriteData(0x11);
    ili9341WriteData(0x07);
    ili9341WriteData(0x31);
    ili9341WriteData(0xC1);
    ili9341WriteData(0x48);
    ili9341WriteData(0x08);
    ili9341WriteData(0x0F);
    ili9341WriteData(0x0C);
    ili9341WriteData(0x31);
    ili9341WriteData(0x36);
    ili9341WriteData(0x0F);

    ili9341SendCmd(ILI9341_SLPOUT);
    _delay_ms(100);

    ili9341SendCmd(ILI9341_DISPON);
    _delay_ms(100);
}

void ili9341SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ili9341SendCmd(ILI9341_PASET);
    ili9341SendData(x);
    ili9341SendData(x + w - 1);

    ili9341SendCmd(ILI9341_CASET);
    ili9341SendData(y);
    ili9341SendData(y + h - 1);

    ili9341SendCmd(ILI9341_RAMWR);
}

static void ili9341Rotate()
{
    ili9341SendCmd(ILI9341_MADCTL);

//    switch (orientation) {
//    case LCD_Orientation_Portrait_1:
//        ili9341WriteData(0x78);
//        glcdOpts.width = ILI9341_WIDTH;
//        glcdOpts.height = ILI9341_HEIGHT;
//        glcdOpts.orientation = orientation;
//        break;
//    case LCD_Orientation_Portrait_2:
//        ili9341WriteData(0xA8);
//        glcdOpts.width = ILI9341_WIDTH;
//        glcdOpts.height = ILI9341_HEIGHT;
//        glcdOpts.orientation = orientation;
//        break;
//    case LCD_Orientation_Landscape_1:
        ili9341WriteData(0x08);
//        glcdOpts.width = ILI9341_HEIGHT;
//        glcdOpts.height = ILI9341_WIDTH;
//        glcdOpts.orientation = orientation;
//        break;
//    default:
//        ili9341WriteData(0xC8);
//        glcdOpts.width = ILI9341_HEIGHT;
//        glcdOpts.height = ILI9341_WIDTH;
//        glcdOpts.orientation = orientation;
//        break;
//    }
}

static void ili9341InitSPI()
{
    return;
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    LL_SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 10;
    LL_SPI_Init(SPI1, &SPI_InitStruct);
}

void ili9341Init(DisplayDriver **disp)
{
    *disp = &drv;
    gc320x240Init(*disp);

    // Configure Hardware SPI
    ili9341InitSPI();

    CLR(ILI9341_RESET);
    _delay_ms(100);
    SET(ILI9341_RESET);

    // Init magic
    ili9341InitSeq();
    SET(ILI9341_LED);

//    // Set display orientation and size
    ili9341Rotate();
}

void ili9341Clear(void)
{
    ili9341DrawRectangle(0, 0, drv.layout->width, drv.layout->height, LCD_COLOR_BLACK);
}

void ili9341Sleep(void)
{
    CLR(ILI9341_LED);
    _delay_ms(120);
    ili9341SendCmd(ILI9341_SLPIN);
    _delay_ms(5);
}

void ili9341Wakeup(void)
{
    _delay_ms(120);
    ili9341SendCmd(ILI9341_SLPOUT);
    _delay_ms(5);
    SET(ILI9341_LED);
}

void ili9341DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    ili9341SetWindow(x, y, 1, 1);
    ili9341SendData(color);
}

void ili9341DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint8_t colorH = color >> 8;
    uint8_t colorL = color & 0xFF;

    ili9341SetWindow(x, y, w, h);

    CLR(ILI9341_CS);
    for (uint16_t i = 0; i < w; i++) {
        for (uint16_t j = 0; j < h; j++) {
            ili9341SendSPI(colorH);
            ili9341SendSPI(colorL);
        }
    }
    // Wait for last transmission to complete
    _delay_us(1);
//    while (LL_SPI_IsActiveFlag_TXE(SPI1));
//    while (!(SPSR & (1 << SPIF)));
    SET(ILI9341_CS);
}

void ili9341DrawFontChar(CharParam *param)
{

}

void ili9341WriteChar(const uint8_t *chOft, uint8_t fwd, uint8_t swd)
{
    uint8_t i;
    uint8_t j;
    uint8_t k;
    uint8_t pgmData;
    uint8_t h = drv.font.data[FONT_HEIGHT];
    uint16_t color = drv.font.color;
    uint16_t bgColor = drv.layout->color;

    CLR(ILI9341_CS);
    for (i = 0; i < fwd; i++) {
        for (j = 0; j < h; j++) {
            if (i >= swd)
                pgmData = 0x00;
            else
                pgmData = chOft[swd * j + i];
            for (k = 0; k < 8; k++) {
                if (pgmData & 0x01) {
                    ili9341SendSPI(color >> 8);
                    ili9341SendSPI(color & 0xFF);
                } else {
                    ili9341SendSPI(bgColor >> 8);
                    ili9341SendSPI(bgColor & 0xFF);
                }
                pgmData >>= 1;
            }
        }
    }
    // Wait for last transmission to complete
    _delay_us(1);
//    while (LL_SPI_IsActiveFlag_TXE(SPI1));
//    while (!(SPSR & (1 << SPIF)));
    SET(ILI9341_CS);
}
