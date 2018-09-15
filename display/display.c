#include "display.h"

#include "../pins.h"

static uint8_t brightness;
static GlcdDriver *glcd;

void displayInit(GlcdDriver **driver)
{
#if defined (_KS0108)
    ks0108Init(driver);
#elif defined (_ST7920)
    st7920Init(driver);
#elif defined (_SSD1306)
    ssd1306Init(driver);
#elif defined (_ILI9163)
    ili9163Init(driver);
#elif defined (_ST7735)
    st7735Init(driver);
#elif defined (_LS020)
    ls020Init(driver);
#elif defined (_LPH9157)
    lph9157Init(driver);
#elif defined (_SSD1286A)
    ssd1286aInit(driver);
#elif defined (_HX8340)
    hx8340Init(driver);
#elif defined (_ILI9320)
    ili9320Init(driver);
#elif defined (_ILI9341)
    ili9341Init(driver);
#elif defined (_S6D0139)
    s6d0139Init(driver);
#elif defined (_SPFD5408)
    spfd5408Init(driver);
#elif defined (_MC2PA8201)
    mc2pa8201Init(driver);
#elif defined (_ILI9327)
    ili9327Init(driver);
#elif defined (_ST7793)
    st7793Init(driver);
#else
#error "Unsupported display driver"
#endif

    glcd = *driver;
}

void displayPWM(void)
{
    static uint8_t br;

    if (++br >= GLCD_MAX_BRIGHTNESS)
        br = GLCD_MIN_BRIGHTNESS;

    if (br == brightness) {
        CLR(DISP_BCKL);
    } else if (br == 0) {
        SET(DISP_BCKL);
    }
}

void displaySetBrightness(uint8_t value)
{
    brightness = value;
}

uint8_t displayGetInput(void)
{
    uint8_t bus = 0;

#if defined(_DISP_8BIT) || defined(_DISP_8BIT_A) || defined(_DISP_8BIT_B)
    bus = glcd->bus;
#elif defined(_DISP_SPI) || defined(_DISP_I2C)
    bus = INPUT_Port->IDR & 0x00FF;   // Read 8-bit bus
#else
#error "Unsupported display driver"
#endif

    return ~bus;
}
