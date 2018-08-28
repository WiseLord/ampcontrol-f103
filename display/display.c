#include "display.h"

#include "../pins.h"

static uint8_t brightness;

void displayInit(GlcdDriver **driver)
{
#if defined (_KS0108)
    ks0108Init(driver);
#elif defined (_ST7920)
    st7920Init(driver);
#elif defined (_SSD1306)
    ssd1306Init(driver);
#elif defined (_LS020)
    ls020Init(driver);
#elif defined (_LPH9157)
    lph9157Init(driver);
#elif defined (_SSD1286A)
    ssd1286aInit(driver);
#elif defined (_ILI9320)
    ili9320Init(driver);
#elif defined (_ILI9341)
    ili9341Init(driver);
#elif defined (_S6D0139)
    s6d0139Init(driver);
#elif defined (_SPFD5408)
    spfd5408Init(driver);
#else
#error "Unsupported display driver"
#endif
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

#if defined(_KS0108) || defined(_ST7920) || defined(_ILI9320) || defined(_S6D0139) || defined(_SPFD5408)
    bus = glcdGetBus();
#elif defined(_ILI9341) || defined(_SSD1306) || defined(_LS020) || defined(_LPH9157) || defined(_SSD1286A)
    bus = INPUT_Port->IDR & 0x00FF;   // Read 8-bit bus
#else
#error "Unsupported display driver"
#endif

    return ~bus;
}
