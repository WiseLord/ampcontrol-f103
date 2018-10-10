#include "ili9341.h"

#include "../dispdrv.h"
#include "../../pins.h"
#include "../../functions.h"

#define ILI9341_WIDTH           240
#define ILI9341_HEIGHT          320
#define ILI9341_PIXELS          (ILI9341_WIDTH * ILI9341_HEIGHT)

// Level 1 commands
#define ILI9341_NOP             0x00 // No Operation
#define ILI9341_SWRESET         0x01 // Software Reset
#define ILI9341_RDDIDIF         0x04 // Read Display Identification Information
#define ILI9341_RDDST           0x09 // Read Display Status
#define ILI9341_RDDPM           0x0A // Read Display Power Mode
#define ILI9341_RDDMADCTL       0x0B // Read Display MADCTL
#define ILI9341_RDDCOLMOD       0x0C // Read Display Pixel Format
#define ILI9341_RDDIM           0x0D // Read Display Image Format
#define ILI9341_RDDSM           0x0D // Read Display Signal mode
#define ILI9341_RDDSDR          0x0D // Read Display Self-Diagnostic Result
#define ILI9341_SLPIN           0x10 // Enter Sleep Mode
#define ILI9341_SLPOUT          0x11 // Sleep Out
#define ILI9341_PTLON           0x12 // Partial Mode ON
#define ILI9341_NORON           0x13 // Normal Display Mode ON
#define ILI9341_DINVOFF         0x20 // Display Inversion OFF
#define ILI9341_DINVON          0x21 // Display Inversion ON
#define ILI9341_GAMSET          0x26 // Gamma Set
#define ILI9341_DISPOFF         0x28 // Display OFF
#define ILI9341_DISPON          0x29 // Display ON
#define ILI9341_CASET           0x2A // Column Address Set
#define ILI9341_PASET           0x2B // Page Address Set
#define ILI9341_RAMWR           0x2C // Memory Write
#define ILI9341_RGBSET          0x2D // Color Set
#define ILI9341_RAMRD           0x2E // Memory Read
#define ILI9341_PLTAR           0x30 // Partial Area
#define ILI9341_VSCRDEF         0x33 // Vertical Scrolling Definition
#define ILI9341_TEOFF           0x34 // Tearing Effect Line OFF
#define ILI9341_TEON            0x35 // Tearing Effect Line ON
#define ILI9341_MADCTL          0x36 // Memory Access Control
#define ILI9341_VSCRSADD        0x37 // Vertical Scrolling Start Address
#define ILI9341_IDMOFF          0x38 // Idle Mode OFF
#define ILI9341_IDMON           0x39 // Idle Mode ON
#define ILI9341_PIXSET          0x3A // COLMOD: Pixel Format Set
#define ILI9341_WRMEMCONT       0x3C // Write Memory Continue
#define ILI9341_RDMEMCONT       0x3E // Read Memory Continue
#define ILI9341_SETTRSCLN       0x44 // Set Tear Scanline
#define ILI9341_GETSCLN         0x45 // Get Scanline
#define ILI9341_WRDISBV         0x51 // Write Display Brightness
#define ILI9341_RDDISBV         0x52 // Read Display Brightness
#define ILI9341_WRCTRLD         0x53 // Write CTRL Display
#define ILI9341_RDCTRLD         0x54 // Read CTRL Display
#define ILI9341_WRCABC          0x55 // Write Content Adaptive Brightness Control
#define ILI9341_RDCABC          0x56 // Read Content Adaptive Brightness Control
#define ILI9341_WRCABCMIN       0x5E // Write CABC Minimum Brightness
#define ILI9341_RDCABCMIN       0x5F // Read CABC Minimum Brightness
#define ILI9341_RDID1           0xDA // Read ID1
#define ILI9341_RDID2           0xDB // Read ID2
#define ILI9341_RDID3           0xDC // Read ID3

// Level 2 commands
#define ILI9341_IFMODE          0xB0 // RGB Interface Signal Control
#define ILI9341_FRMCTR1         0xB1 // Frame Rate Control (In Normal Mode / Full Colors)
#define ILI9341_FRMCTR2         0xB2 // Frame Rate Control (In Idle Mode / 8 Colors)
#define ILI9341_FRMCTR3         0xB3 // Frame Rate Control (In Partial Mode / Full Colors)
#define ILI9341_INVCTR          0xB4 // Display Inversion Control
#define ILI9341_PRCTR           0xB5 // Blanking Porch
#define ILI9341_DISCTRL         0xB6 // Display Function Control
#define ILI9341_ETMOD           0xB7 // Entry Mode Set
#define ILI9341_BCKLCTR1        0xB8 // Backlight Control 1
#define ILI9341_BCKLCTR2        0xB9 // Backlight Control 2
#define ILI9341_BCKLCTR3        0xBA // Backlight Control 3
#define ILI9341_BCKLCTR4        0xBB // Backlight Control 4
#define ILI9341_BCKLCTR5        0xBC // Backlight Control 5
#define ILI9341_BCKLCTR7        0xBE // Backlight Control 7
#define ILI9341_BCKLCTR8        0xBF // Backlight Control 8
#define ILI9341_PWCTRL1         0xC0 // Power Control 1
#define ILI9341_PWCTRL2         0xC1 // Power Control 2
#define ILI9341_PWCTRL3         0xC2 // Power Control 3 (For Normal Mode)
#define ILI9341_PWCTRL4         0xC3 // Power Control 4 (For Idle Mode)
#define ILI9341_PWCTRL5         0xC4 // Power Control 4 (For Partial Mode)
#define ILI9341_VMCTRL1         0xC5 // VCOM Control 1
#define ILI9341_VMCTRL2         0xC7 // VCOM Control 2

#define ILI9341_NVMWR           0xD0 // NV Memory Write
#define ILI9341_NVMPKEY         0xD1 // NV Memory Protection Key
#define ILI9341_RDNVM           0xD2 // NV Memory Status Read
#define ILI9341_RDID4           0xD3 // Read ID4
#define ILI9341_PGAMCTRL        0xE0 // Positive Gamma Control
#define ILI9341_NGAMCTRL        0xE1 // Negative Gamma Control
#define ILI9341_DGAMCTRL1       0xE2 // Digital Gamma Control 1
#define ILI9341_DGAMCTRL2       0xE3 // Digital Gamma Control 2
#define ILI9341_IFCTL           0xF6 // 16bits Data Format Selection

// Extend register command
#define ILI9341_PWCTRLA         0xCB // Power Control A
#define ILI9341_PWCTRLB         0xCF // Power Control B
#define ILI9341_DRVTIMCTLA1     0xE8 // Driver Timing Control A 1
#define ILI9341_DRVTIMCTLA2     0xE9 // Driver Timing Control A 2
#define ILI9341_DRVTIMB         0xEA // Driver Timing Control B
#define ILI9341_PWSEQCTL        0xED // Power On Sequence Control
#define ILI9341_EN3G            0xF2 // Enable 3G
#define ILI9341_PUMPRTCTL       0xF7 // Pump Ratio Control

static GlcdDriver glcd = {
    .clear = ili9341Clear,
    .drawPixel = ili9341DrawPixel,
    .drawRectangle = ili9341DrawRectangle,
    .drawImage = ili9341DrawImage,
};

static inline void ili9341SendCmd(uint8_t cmd) __attribute__((always_inline));
static inline void ili9341SendCmd(uint8_t cmd)
{
    dispdrvWaitOperation();
    CLR(DISP_SPI_DC);

    dispdrvSendData8(cmd);

    dispdrvWaitOperation();
    SET(DISP_SPI_DC);
}

static void ili9341InitSeq(void)
{
    _delay_ms(50);

    CLR(DISP_SPI_CS);

    ili9341SendCmd(ILI9341_SWRESET);
    _delay_ms(10);
    ili9341SendCmd(ILI9341_DISPOFF);

    ili9341SendCmd(ILI9341_PWCTRLB);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0xC1);
    dispdrvSendData8(0x30);

    ili9341SendCmd(ILI9341_PWSEQCTL);
    dispdrvSendData8(0x64);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x81);

    ili9341SendCmd(ILI9341_DRVTIMCTLA1);
    dispdrvSendData8(0x85);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x79);

    ili9341SendCmd(ILI9341_PWCTRLA);
    dispdrvSendData8(0x39);
    dispdrvSendData8(0x2C);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x34);
    dispdrvSendData8(0x02);

    ili9341SendCmd(ILI9341_PUMPRTCTL);
    dispdrvSendData8(0x20);

    ili9341SendCmd(ILI9341_DRVTIMB);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    ili9341SendCmd(ILI9341_PWCTRL1);
    dispdrvSendData8(0x26);

    ili9341SendCmd(ILI9341_PWCTRL2);
    dispdrvSendData8(0x11);

    ili9341SendCmd(ILI9341_VMCTRL1);
    dispdrvSendData8(0x35);
    dispdrvSendData8(0x3E);

    ili9341SendCmd(ILI9341_VMCTRL2);
    dispdrvSendData8(0xBE);

    ili9341SendCmd(ILI9341_PIXSET);
    dispdrvSendData8(0x55);

    ili9341SendCmd(ILI9341_FRMCTR1);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x1B);

    ili9341SendCmd(ILI9341_EN3G);
    dispdrvSendData8(0x08);

    ili9341SendCmd(ILI9341_GAMSET);
    dispdrvSendData8(0x01);

    ili9341SendCmd(ILI9341_ETMOD);
    dispdrvSendData8(0x07);

    ili9341SendCmd(ILI9341_DISCTRL);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x82);
    dispdrvSendData8(0x27);

    // Retate
    ili9341SendCmd(ILI9341_MADCTL);
    dispdrvSendData8(0x08);

    ili9341SendCmd(ILI9341_PGAMCTRL);
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0x2B);
    dispdrvSendData8(0x0C);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x4E);
    dispdrvSendData8(0xF1);
    dispdrvSendData8(0x37);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x09);
    dispdrvSendData8(0x00);

    ili9341SendCmd(ILI9341_NGAMCTRL);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x0E);
    dispdrvSendData8(0x14);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x11);
    dispdrvSendData8(0x07);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0xC1);
    dispdrvSendData8(0x48);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x0C);
    dispdrvSendData8(0x31);
    dispdrvSendData8(0x36);
    dispdrvSendData8(0x0F);

    ili9341SendCmd(ILI9341_SLPOUT);
    ili9341SendCmd(ILI9341_DISPON);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ili9341SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ili9341SendCmd(ILI9341_PASET);
    dispdrvSendData16(x);
    dispdrvSendData16(x + w - 1);

    ili9341SendCmd(ILI9341_CASET);
    dispdrvSendData16(y);
    dispdrvSendData16(y + h - 1);

    ili9341SendCmd(ILI9341_RAMWR);
}

void ili9341Init(GlcdDriver **driver)
{
    *driver = &glcd;
    gc320x240Init(*driver);

    CLR(DISP_SPI_RST);
    _delay_ms(100);
    SET(DISP_SPI_RST);

    // Init magic
    ili9341InitSeq();
    SET(DISP_SPI_LED);
}

void ili9341Clear(void)
{
    ili9341DrawRectangle(0, 0, glcd.canvas->width, glcd.canvas->height, glcd.canvas->color);
}

void ili9341Sleep(void)
{
    CLR(DISP_SPI_CS);

    ili9341SendCmd(ILI9341_SLPIN);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ili9341Wakeup(void)
{
    CLR(DISP_SPI_CS);

    ili9341SendCmd(ILI9341_SLPOUT);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ili9341DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_SPI_CS);

    ili9341SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ili9341DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    CLR(DISP_SPI_CS);

    ili9341SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}

void ili9341DrawImage(tImage *img)
{
    uint16_t w = img->width;
    uint16_t h = img->height;
    uint16_t x0 = glcd.canvas->x;
    uint16_t y0 = glcd.canvas->y;

    CLR(DISP_SPI_CS);

    ili9341SetWindow(x0, y0, w, h);
    dispdrvSendImage(img, w, h);

    dispdrvWaitOperation();
    SET(DISP_SPI_CS);
}
