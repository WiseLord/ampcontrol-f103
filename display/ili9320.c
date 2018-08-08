#include "ili9320.h"

#include "../pins.h"
#include "../functions.h"

static uint8_t pins;
static uint8_t bus_requested = 0;

static LCD_Options ILI9320_Opts;

void ILI9320_Init(void)
{
    SET(ILI9320_LED);
    SET(ILI9320_RD);
    SET(ILI9320_WR);
    SET(ILI9320_RS);
    CLR(ILI9320_CS);

    SET(ILI9320_RESET);
    _delay_ms(1);
    CLR(ILI9320_RESET);
    _delay_ms(10);
    SET(ILI9320_RESET);

    ILI9320_Opts.width = ILI9320_WIDTH;
    ILI9320_Opts.height = ILI9320_HEIGHT;
    ILI9320_Opts.orientation = LCD_Orientation_Portrait_1;

    ILI9320_InitLCD();
}

void ILI9320_IRQ()
{
    bus_requested = 1;
}

uint8_t ILI9320_GetPins(void)
{
    return ~pins;
}


void ILI9320_Write(uint16_t data)
{
    ILI9320_DHI_Port->BSRR = 0x00FF0000 | (data >> 8);              // If port bits 7..0 are used
    CLR(ILI9320_WR);                                                // Strob MSB
    SET(ILI9320_WR);
    ILI9320_DHI_Port->BSRR = 0x00FF0000 | (data & 0x00FF);          // If port bits 7..0 are used
    CLR(ILI9320_WR);                                                // Strob LSB
    SET(ILI9320_WR);

    // If input IRQ requested bus status, switch temporarly to input mode and read bus
    if (bus_requested) {
        ILI9320_DHI_Port->BSRR = 0x000000FF;                        // Set 1 on all data lines
        ILI9320_DHI_Port->CRL = 0x88888888;                         // SET CNF=10, MODE=00 - Input pullup
        _delay_us(5);
        pins = ILI9320_DHI_Port->IDR & 0x00FF;
        ILI9320_DHI_Port->CRL = 0x33333333;                         // Set CNF=00, MODE=11 - Output push-pull 50 MHz
        bus_requested = 0;
    }
}

void ILI9320_SelectReg(uint16_t reg)
{
    CLR(ILI9320_RS);
    ILI9320_Write(reg);
    SET(ILI9320_RS);
}

void ILI9320_WriteReg(uint16_t reg, uint16_t data)
{
    ILI9320_SelectReg(reg);
    ILI9320_Write(data);
}


void ILI9320_InitLCD(void)
{
    // Wait for reset
    _delay_ms(50);

    // Initial Sequence
    ILI9320_WriteReg(0x00E5, 0x8000); // Set the Vcore voltage and this setting is must
    ILI9320_WriteReg(0x0000, 0x0001); // Start internal OSC
    ILI9320_WriteReg(0x0001, 0x0000); // Set SS and SM bit
    ILI9320_WriteReg(0x0002, 0x0700); // Set 1 line inversion
    ILI9320_WriteReg(0x0003, 0x1030); // Set GRAM write direction and BGR = 1
    ILI9320_WriteReg(0x0004, 0x0000); // Resize register

    ILI9320_WriteReg(0x0008, 0x0202); // Set the back porch and front porch
    ILI9320_WriteReg(0x0009, 0x0000); // Set non-display area refresh cycle ICS[3:0]
    ILI9320_WriteReg(0x000A, 0x0000); // FMARK function
    ILI9320_WriteReg(0x000C, 0x0000); // RGB interface setting
    ILI9320_WriteReg(0x000D, 0x0000); // Frame marker position
    ILI9320_WriteReg(0x000F, 0x0000); // RGB interface polarity

    // Power On Sequence
    ILI9320_WriteReg(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
    ILI9320_WriteReg(0x0011, 0x0000); // DC1[2:0], DC0[2:0], VC[2:0]
    ILI9320_WriteReg(0x0012, 0x0000); // VREG1OUT voltage
    ILI9320_WriteReg(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
    _delay_ms(200);
    ILI9320_WriteReg(0x0010, 0x17B0); // SAP, BT[3:0], AP, DSTB, SLP, STB
    ILI9320_WriteReg(0x0011, 0x0037); // DC1[2:0], DC0[2:0], VC[2:0]
    _delay_ms(50);
    ILI9320_WriteReg(0x0012, 0x013A); // VREG1OUT voltage
    _delay_ms(50);
    ILI9320_WriteReg(0x0013, 0x1600); // VDV[4:0] for VCOM amplitude
    ILI9320_WriteReg(0x0029, 0x000C); // VCM[4:0] for VCOMH
    _delay_ms(50);
    ILI9320_WriteReg(0x0020, 0x0000); // GRAM horisontal address
    ILI9320_WriteReg(0x0021, 0x0000); // GRAM vertical address

    // Adjust the Gamma Curve
    ILI9320_WriteReg(0x0030, 0x0007);
    ILI9320_WriteReg(0x0031, 0x0700);
    ILI9320_WriteReg(0x0032, 0x0707);
    ILI9320_WriteReg(0x0035, 0x0007);
    ILI9320_WriteReg(0x0036, 0x000F);
    ILI9320_WriteReg(0x0037, 0x0700);
    ILI9320_WriteReg(0x0038, 0x0707);
    ILI9320_WriteReg(0x0039, 0x0007);
    ILI9320_WriteReg(0x003C, 0x0700);
    ILI9320_WriteReg(0x003D, 0x0F00);

    // Set GRAM area
    ILI9320_WriteReg(0x0050, 0x0000); // Horizontal GRAM Start Address
    ILI9320_WriteReg(0x0051, 0x00EF); // Horizontal GRAM End Address
    ILI9320_WriteReg(0x0052, 0x0000); // Vertical GRAM Start Address
    ILI9320_WriteReg(0x0053, 0x013F); // Vertical GRAM End Address

    ILI9320_WriteReg(0x0060, 0x2700); // Gate scan line
    ILI9320_WriteReg(0x0061, 0x0003); // NDV, VLE, REV
    ILI9320_WriteReg(0x006A, 0x0000); // Set scrolling line

    // Partial Display Control
    ILI9320_WriteReg(0x0080, 0x0000);
    ILI9320_WriteReg(0x0081, 0x0000);
    ILI9320_WriteReg(0x0082, 0x0000);
    ILI9320_WriteReg(0x0083, 0x0000);
    ILI9320_WriteReg(0x0084, 0x0000);
    ILI9320_WriteReg(0x0085, 0x0000);

    // Panel Control
    ILI9320_WriteReg(0x0090, 0x0010);
    ILI9320_WriteReg(0x0092, 0x0000);
    ILI9320_WriteReg(0x0093, 0x0003);
    ILI9320_WriteReg(0x0095, 0x0110);
    ILI9320_WriteReg(0x0097, 0x0000);
    ILI9320_WriteReg(0x0098, 0x0000);

    ILI9320_WriteReg(0x0007, 0x0173); // 262K color and display ON
}

void ILI9320_ExitSleep(void)
{
    // Power On Sequence
    ILI9320_WriteReg(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
    ILI9320_WriteReg(0x0011, 0x0000); // DC1[2:0], DC0[2:0], VC[2:0]
    ILI9320_WriteReg(0x0012, 0x0000); // VREG1OUT voltage
    ILI9320_WriteReg(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
    _delay_ms(200);
    ILI9320_WriteReg(0x0010, 0x17B0); // SAP, BT[3:0], AP, DSTB, SLP, STB
    ILI9320_WriteReg(0x0011, 0x0037); // DC1[2:0], DC0[2:0], VC[2:0]
    _delay_ms(50);
    ILI9320_WriteReg(0x0012, 0x013A); // VREG1OUT voltage
    _delay_ms(50);
    ILI9320_WriteReg(0x0013, 0x1600); // VDV[4:0] for VCOM amplitude
    ILI9320_WriteReg(0x0029, 0x000C); // VCM[4:0] for VCOMH

    ILI9320_WriteReg(0x0007, 0x0173); // 262K color and display ON
}

void ILI9320_EnterSleep(void)
{
    ILI9320_WriteReg(0x0007, 0x0000); // Display OFF
    // Power Off Sequence
    ILI9320_WriteReg(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
    ILI9320_WriteReg(0x0011, 0x0000); // DC1[2:0], DC0[2:0], VC[2:0]
    ILI9320_WriteReg(0x0012, 0x0000); // VREG1OUT voltage
    ILI9320_WriteReg(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
    _delay_ms(200);
    ILI9320_WriteReg(0x0010, 0x0002); // SAP, BT[3:0], AP, DSTB, SLP, STB
}

void ILI9320_Rotate(LCD_Orientation orientation)
{
    switch (orientation) {
    case LCD_Orientation_Portrait_1:
        ILI9320_WriteReg(0x0003, 0x1030); // Set GRAM write direction and BGR = 1
        ILI9320_Opts.width = ILI9320_WIDTH;
        ILI9320_Opts.height = ILI9320_HEIGHT;
        ILI9320_Opts.orientation = orientation;
        break;
    case LCD_Orientation_Portrait_2:
        ILI9320_WriteReg(0x0003, 0x1000); // Set GRAM write direction and BGR = 1
        ILI9320_Opts.width = ILI9320_WIDTH;
        ILI9320_Opts.height = ILI9320_HEIGHT;
        ILI9320_Opts.orientation = orientation;
        break;
    case LCD_Orientation_Landscape_1:
        ILI9320_WriteReg(0x0003, 0x1028); // Set GRAM write direction and BGR = 1
        ILI9320_Opts.width = ILI9320_HEIGHT;
        ILI9320_Opts.height = ILI9320_WIDTH;
        ILI9320_Opts.orientation = orientation;
        break;
    case LCD_Orientation_Landscape_2:
        ILI9320_WriteReg(0x0003, 0x1018); // Set GRAM write direction and BGR = 1
        ILI9320_Opts.width = ILI9320_HEIGHT;
        ILI9320_Opts.height = ILI9320_WIDTH;
        ILI9320_Opts.orientation = orientation;
    default:
        break;
    }
}


void ILI9320_SetCursor(uint16_t x, uint16_t y)
{
    switch (ILI9320_Opts.orientation) {
    case LCD_Orientation_Portrait_1:
        ILI9320_WriteReg(0x0020, x);
        ILI9320_WriteReg(0x0021, y);
        break;
    case LCD_Orientation_Portrait_2:
        ILI9320_WriteReg(0x0020, ILI9320_WIDTH - x - 1);
        ILI9320_WriteReg(0x0021, ILI9320_HEIGHT - y - 1);
        break;
    case LCD_Orientation_Landscape_1:
        ILI9320_WriteReg(0x0020, ILI9320_WIDTH - y - 1);
        ILI9320_WriteReg(0x0021, x);
        break;
    case LCD_Orientation_Landscape_2:
        ILI9320_WriteReg(0x0020, y);
        ILI9320_WriteReg(0x0021, ILI9320_HEIGHT - x - 1);
        break;
    default:
        break;
    }
}

void ILI9320_SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ILI9320_SetCursor(x, y);

    switch (ILI9320_Opts.orientation) {
    case LCD_Orientation_Portrait_1:
        ILI9320_WriteReg(0x0050, x);
        ILI9320_WriteReg(0x0051, x + w - 1);
        ILI9320_WriteReg(0x0052, y);
        ILI9320_WriteReg(0x0053, y + h - 1);
        break;
    case LCD_Orientation_Portrait_2:
        ILI9320_WriteReg(0x0050, ILI9320_WIDTH - x - w);
        ILI9320_WriteReg(0x0051, ILI9320_WIDTH - x - 1);
        ILI9320_WriteReg(0x0052, ILI9320_HEIGHT - y - h);
        ILI9320_WriteReg(0x0053, ILI9320_HEIGHT - y - 1);
        break;
    case LCD_Orientation_Landscape_1:
        ILI9320_WriteReg(0x0050, ILI9320_WIDTH - y - h);
        ILI9320_WriteReg(0x0051, ILI9320_WIDTH - y - 1);
        ILI9320_WriteReg(0x0052, x);
        ILI9320_WriteReg(0x0053, x + w - 1);
        break;
    case LCD_Orientation_Landscape_2:
        ILI9320_WriteReg(0x0050, y);
        ILI9320_WriteReg(0x0051, y + h - 1);
        ILI9320_WriteReg(0x0052, ILI9320_HEIGHT - x - w);
        ILI9320_WriteReg(0x0053, ILI9320_HEIGHT - x - 1);
        break;
    default:
        break;
    }
}


void ILI9320_Fill(uint16_t color)
{
    uint32_t i;

    ILI9320_SetWindow(0, 0, ILI9320_Opts.width, ILI9320_Opts.height);

    ILI9320_SelectReg(0x0022);
    for (i = 0; i < ILI9341_PIXEL; i++)
        ILI9320_Write(color);
}

void ILI9320_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= ILI9320_Opts.width || y >= ILI9320_Opts.height)
        return;

    ILI9320_SetCursor(x, y);

    ILI9320_SelectReg(0x0022);
    ILI9320_Write(color);
}

void ILI9320_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    int16_t sX, sY, dX, dY, err, err2;

    sX = x0 < x1 ? 1 : -1;
    sY = y0 < y1 ? 1 : -1;
    dX = sX > 0 ? x1 - x0 : x0 - x1;
    dY = sY > 0 ? y1 - y0 : y0 - y1;
    err = dX - dY;

    while (x0 != x1 || y0 != y1) {
        ILI9320_DrawPixel(x0, y0, color);
        err2 = err * 2;
        if (err2 > -dY / 2) {
            err -= dY;
            x0 += sX;
        }
        if (err2 < dX) {
            err += dX;
            y0 += sY;
        }
    }
    ILI9320_DrawPixel(x1, y1, color);
}

void ILI9320_DrawHorizLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color)
{
    uint16_t i, len;

    len = x1 - x0 + 1;

    ILI9320_SetWindow(x0, y, len, 1);
    ILI9320_SelectReg(0x0022);
    for (i = 0; i < len; i++)
        ILI9320_Write(color);
}

void ILI9320_DrawVertLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color)
{
    uint16_t i, len;

    len = y1 - y0 + 1;

    ILI9320_SetWindow(x, y0, 1, len);
    ILI9320_SelectReg(0x0022);
    for (i = 0; i < len; i++)
        ILI9320_Write(color);
}

void ILI9320_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    ILI9320_DrawHorizLine(x0, x1, y0, color);
    ILI9320_DrawVertLine(x0, y0, y1, color);
    ILI9320_DrawVertLine(x1, y0, y1, color);
    ILI9320_DrawHorizLine(x0, x1, y1, color);
}

void ILI9320_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t w, h;

    w = x1 - x0 + 1;
    h = y1 - y0 + 1;

    ILI9320_SetWindow(x0, y0, w, h);
    ILI9320_SelectReg(0x0022);
    for (uint32_t i = 0; i < w * h; i++)
        ILI9320_Write(color);
}

void ILI9320_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    ILI9320_DrawPixel(x0, y0 + r, color);
    ILI9320_DrawPixel(x0, y0 - r, color);
    ILI9320_DrawPixel(x0 + r, y0, color);
    ILI9320_DrawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ILI9320_DrawPixel(x0 + x, y0 + y, color);
        ILI9320_DrawPixel(x0 - x, y0 + y, color);
        ILI9320_DrawPixel(x0 + x, y0 - y, color);
        ILI9320_DrawPixel(x0 - x, y0 - y, color);

        ILI9320_DrawPixel(x0 + y, y0 + x, color);
        ILI9320_DrawPixel(x0 - y, y0 + x, color);
        ILI9320_DrawPixel(x0 + y, y0 - x, color);
        ILI9320_DrawPixel(x0 - y, y0 - x, color);
    }
}

void ILI9320_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    ILI9320_DrawHorizLine(x0 - r, x0 + r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ILI9320_DrawHorizLine(x0 - x, x0 + x, y0 + y, color);
        ILI9320_DrawHorizLine(x0 - x, x0 + x, y0 - y, color);
        ILI9320_DrawHorizLine(x0 - y, x0 + y, y0 + x, color);
        ILI9320_DrawHorizLine(x0 - y, x0 + y, y0 - x, color);
    }
}
