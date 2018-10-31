#include "canvas.h"

Canvas *canvas;

#define COLOR_CANVAS    LCD_COLOR_BLACK

#define COLOR_SPECTRUM_COLUMN   LCD_COLOR_ELECTRIC_BLUE
#define COLOR_SPECTRUM_PEAK     LCD_COLOR_WITCH_HAZE

void canvasInit(Canvas **value)
{
#if defined (_KS0108A) || defined(_KS0108B) || defined (_ST7920) || defined (_SSD1306)
    gm128x64Init(value);
#elif defined (_ILI9163) || defined (_ST7735)
    gc160x128Init(value);
#elif defined (_LS020) || defined (_LPH9157) || defined (_SSD1286A)
    gc176x132Init(value);
#elif defined (_HX8340) || defined (_ILI9225)
    gc220x176Init(value);
#elif defined (_ILI9320) || defined (_ILI9341) || defined (_S6D0139) || defined (_SPFD5408) || defined (_MC2PA8201)
    gc320x240Init(value);
#elif defined (_ILI9327) || defined (_ST7793)
    gc400x240Init(value);
#elif defined (_ILI9481) || defined (_R61581)
    gc480x320Init(value);
#else
#ifdef EMUL_DISP
    emulCanvasInit(value);
#else
#error "Unsupported display driver"
#endif
#endif
    (*value)->color = LCD_COLOR_BLACK;

    canvas = *value;
}

void canvasDrawBar(int16_t value, int16_t min, int16_t max, BarParams *bar)
{
    const int16_t sc = bar->sc;         // Scale count
    const uint8_t sw = bar->sw;         // Scale width
    const uint16_t barPos = bar->pos;
    const uint8_t barHalf = bar->half;
    const uint8_t barMiddle = bar->middle;

    if (min + max) { // Non-symmectic scale => rescale to 0..sl
        value = sc * (value - min) / (max - min);
    } else { // Symmetric scale => rescale to -sl/2..sl/2
        value = (sc / 2) * value / (max ? max : 1);
    }

    for (uint16_t i = 0; i < sc; i++) {
        uint16_t color = LCD_COLOR_WHITE;

        if (min + max) { // Non-symmetric scale
            if (i >= value) {
                color = canvas->color;
            }
        } else { // Symmetric scale
            if ((value > 0 && i >= value + (sc / 2)) ||
                (value >= 0 && i < (sc / 2 - 1)) ||
                (value < 0 && i < value + (sc / 2)) ||
                (value <= 0 && i > (sc / 2))) {
                color = canvas->color;
            }
        }

        uint16_t width = canvas->width;

        glcdDrawRect(i * (width / sc) + 1, barPos, sw, barHalf, color);
        glcdDrawRect(i * (width / sc) + 1, barPos + barHalf, sw, barMiddle, LCD_COLOR_WHITE);
        glcdDrawRect(i * (width / sc) + 1, barPos + barHalf + barMiddle, sw, barHalf, color);
    }
}


static void canvasDrawSpectrumColumn(bool clear, uint16_t x, uint16_t y, uint8_t w, uint16_t h,
                                     uint8_t s, uint8_t os, uint8_t p, uint8_t op)
{
    if (s == 0) {
        s = 1;
    }
    if (s >= h) {
        s = h - 1;
    }
    if (p >= h) {
        p = h - 1;
    }
    if (os >= h) {
        os = h - 1;
    }
    if (op >= h) {
        op = h - 1;
    }

    if (clear) {
        glcdDrawRect(x, y + h - s, w, s, COLOR_SPECTRUM_COLUMN);

        if (p > s) {
            glcdDrawRect(x, y + h - p, w, 1, COLOR_SPECTRUM_PEAK);
        }
        return;
    }

    if (s > os) {
        glcdDrawRect(x, y + h - s, w, s - os, COLOR_SPECTRUM_COLUMN);

    } else if (s < os) {
        glcdDrawRect(x, y + h - os, w, os - s, canvas->color);
    }

    if (p > s) {
        glcdDrawRect(x, y + h - p, w, 1, COLOR_SPECTRUM_PEAK);
    }
    if (op > p && op > s) {
        glcdDrawRect(x, y + h - op, w, 1, canvas->color);
    }

}

void canvasShowSpectrum(bool clear, SpectrumData *spData, uint8_t step, uint8_t oft, uint8_t width)
{
    const uint16_t height = canvas->height / 2;                 // Height of spectrum column
    const uint16_t num = (canvas->width + width - 1) / step;    // Number of spectrum columns

    for (uint8_t chan = SP_CHAN_LEFT; chan < SP_CHAN_END; chan++) {
        uint8_t *show = spData[chan].show;
        uint8_t *peak = spData[chan].peak;
        uint8_t *old_show = spData[chan].old_show;
        uint8_t *old_peak = spData[chan].old_peak;

        for (uint16_t col = 0; col < num; col++) {
            uint16_t x = oft + col * step;
            uint16_t y = chan * height;
            canvasDrawSpectrumColumn(clear, x, y, width, height,
                                     *show++, *old_show++, *peak++, *old_peak++);
        }
    }
}


static void canvasDrawMenuItem(uint8_t idx, const tFont *fontItem)
{
    int16_t fIh = fontItem->chars[0].image->height;

    Menu *menu = menuGet();
    uint8_t items = menu->dispSize;

    uint16_t width = canvas->width;
    MenuIdx menuIdx = menu->list[idx];
    char *name = menuGetName(menuIdx);
    uint8_t active = (menu->active == menu->list[idx]);

    const uint8_t ih = fIh + 4; // Menu item height
    uint16_t y_pos = canvas->height - ih * (items - idx + menu->dispOft);

    // Draw selection frame
    glcdDrawFrame(0, y_pos, width - 1, y_pos + ih - 1, active ? LCD_COLOR_WHITE : canvas->color);

    // Draw menu name
    glcdSetFont(fontItem);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(4, y_pos + 2);
    if (menu->list[idx] != MENU_NULL) {
        glcdWriteString("  ");
    } else {
        glcdWriteString("< ");
    }
    glcdWriteString(name);

    // Draw menu value
    uint16_t x = canvas->glcd->x;
    glcdSetXY(width - 2, y_pos + 2);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);

    // Inverse value color if selected
    uint16_t color = canvas->glcd->font.color;
    uint16_t bgColor = canvas->glcd->font.bgColor;
    if (active && menu->selected) {
        glcdSetFontColor(bgColor);
        glcdSetFontBgColor(color);
    }
    uint16_t strLen = glcdWriteStringFramed(menuGetValueStr(menuIdx), 1);
    glcdSetFontColor(color);
    glcdSetFontBgColor(bgColor);

    // Fill space between name and value
    glcdDrawRect(x, y_pos + 2, width - 2 - x - strLen, fIh, canvas->color);
}

void canvasShowTuner(DispTuner *dt, const tFont *fmFont, BarParams *bar)
{
    Tuner *tuner = dt->tuner;
    uint16_t freq = tunerGet()->freq;
    uint16_t freqMin = tuner->par.fMin;
    uint16_t freqMax = tuner->par.fMax;

    glcdSetFont(fmFont);
    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetXY(2, 0);

    glcdWriteString("FM ");

    canvasDrawBar(freq, freqMin, freqMax, bar);

    glcdWriteNum(freq / 100, 3, ' ', 10);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteNum(freq % 100, 2, '0', 10);
}

void canvasShowMenu(const tFont *fontHeader, const tFont *fontItem)
{

    Menu *menu = menuGet();

    int16_t fHh = fontHeader->chars[0].image->height;
    int16_t fIh = fontItem->chars[0].image->height;
    uint8_t items = menu->dispSize;

    int16_t dividerPos = (canvas->height - (fIh + 4) * items + fHh) / 2;

    // Show header
    char *parentName = menuGetName(menu->parent);
    glcdSetFont(fontHeader);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString(parentName);

    glcdDrawRect(0, dividerPos, canvas->width, 1, canvas->glcd->font.color);

    for (uint8_t idx = 0; idx < menu->listSize; idx++) {
        if (idx >= menu->dispOft && idx < items + menu->dispOft) {
            canvasDrawMenuItem(idx, fontItem);
        }
    }
}
