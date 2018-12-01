#include "canvas.h"

static Canvas canvas;

#define COLOR_CANVAS    LCD_COLOR_BLACK

#define COLOR_SPECTRUM_COLUMN   LCD_COLOR_ELECTRIC_BLUE
#define COLOR_SPECTRUM_PEAK     LCD_COLOR_WITCH_HAZE

void canvasInit(void)
{
#if defined (_KS0108A) || defined(_KS0108B) || defined (_ST7920) || defined (_SSD1306)
    lt128x64Init(&canvas);
#elif defined (_ILI9163) || defined (_ST7735)
    lt160x128Init(&canvas);
#elif defined (_LS020) || defined (_LPH9157) || defined (_SSD1286A)
    lt176x132Init(&canvas);
#elif defined (_HX8340) || defined (_ILI9225)
    lt220x176Init(&canvas);
#elif defined (_ILI9320) || defined (_ILI9341) || defined (_S6D0139) || defined (_SPFD5408) || defined (_MC2PA8201)
    lt320x240Init(&canvas);
#elif defined (_ILI9327) || defined (_ST7793)
    lt400x240Init(&canvas);
#elif defined (_ILI9481) || defined (_R61581)
    lt480x320Init(&canvas);
#else
#ifdef EMUL_DISP
    ltEmulInit(&canvas);
#else
#error "Unsupported display driver"
#endif
#endif

    glcdInit(&canvas.glcd);

    // TODO: Read rotate value from settings
    glcdRotate(LCD_ROTATE_0);

    canvas.color = LCD_COLOR_BLACK;
    menuGet()->dispSize = canvas.lt->menu.itemCnt;
}

void canvasClear(void)
{
    glcdDrawRect(0, 0, canvas.lt->width, canvas.lt->height, canvas.color);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(canvas.color);
}

static void canvasDrawBar(const CanvasBar *bar, int16_t value, int16_t min, int16_t max)
{
    const int16_t sc = bar->sc;         // Scale count
    const uint8_t sw = bar->sw;         // Scale width
    const int16_t barPos = bar->barY;
    const uint8_t barHalf = bar->half;
    const uint8_t barMiddle = bar->middle;
    const uint16_t width = bar->barW;

    if (min + max) { // Non-symmectic scale => rescale to 0..sl
        value = sc * (value - min) / (max - min);
    } else { // Symmetric scale => rescale to -sl/2..sl/2
        value = (sc / 2) * value / (max ? max : 1);
    }

    for (uint16_t i = 0; i < sc; i++) {
        uint16_t color = LCD_COLOR_WHITE;

        if (min + max) { // Non-symmetric scale
            if (i >= value) {
                color = canvas.color;
            }
        } else { // Symmetric scale
            if ((value > 0 && i >= value + (sc / 2)) ||
                (value >= 0 && i < (sc / 2 - 1)) ||
                (value < 0 && i < value + (sc / 2)) ||
                (value <= 0 && i > (sc / 2))) {
                color = canvas.color;
            }
        }

        glcdDrawRect(i * (width / sc), barPos, sw, barHalf, color);
        glcdDrawRect(i * (width / sc), barPos + barHalf, sw, barMiddle, LCD_COLOR_WHITE);
        glcdDrawRect(i * (width / sc), barPos + barHalf + barMiddle, sw, barHalf, color);
    }
}

/*
static uint16_t level2color(uint8_t value)
{
    uint16_t color = 0;

    if (value >= 0 && value < 64) {
        color = 0x001F;
        color |= (value << 5);
    } else if (value >= 64 && value < 128) {
        color = 0x07E0;
        color |= (31 - ((value - 64) / 2));
    } else if (value >= 128 && value < 192) {
        color = 0x07E0;
        color |= ((value / 2) << 11);
    } else if (value >= 192 && value < 256) {
        color = 0xF800;
        color |= ((63 - value) << 5);
    }

    return color;
}
*/

static void canvasDrawTm(RTC_type *rtc, uint8_t tm)
{
    int8_t time = *((int8_t *)rtc + tm);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(LCD_COLOR_BLACK);
    if (rtc->etm == tm) {
        glcdSetFontColor(LCD_COLOR_BLACK);
        glcdSetFontBgColor(LCD_COLOR_WHITE);
    }
    glcdWriteChar(LETTER_SPACE_CHAR);
    if (tm == RTC_YEAR) {
        glcdWriteString("20");
        glcdWriteChar(LETTER_SPACE_CHAR);
    }
    glcdWriteNum(time, 2, '0', 10);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(LCD_COLOR_BLACK);
}

static void canvasDrawMenuItem(uint8_t idx, const tFont *fontItem)
{
    uint8_t fIh = (uint8_t)fontItem->chars[0].image->height;

    Menu *menu = menuGet();
    uint8_t items = menu->dispSize;

    int16_t width = canvas.lt->width;
    MenuIdx menuIdx = menu->list[idx];
    char *name = menuGetName(menuIdx);
    uint8_t active = (menu->active == menu->list[idx]);

    const uint8_t ih = fIh + 4; // Menu item height
    int16_t y_pos = canvas.lt->height - ih * (items - idx + menu->dispOft);

    // Draw selection frame
    glcdDrawFrame(0, y_pos, width - 1, y_pos + ih - 1, active ? LCD_COLOR_WHITE : canvas.color);

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
    int16_t x = canvas.glcd->x;
    glcdSetXY(width - 2, y_pos + 2);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);

    // Inverse value color if selected
    uint16_t color = canvas.glcd->font.color;
    uint16_t bgColor = canvas.glcd->font.bgColor;
    if (active && menu->selected) {
        glcdSetFontColor(bgColor);
        glcdSetFontBgColor(color);
    }
    uint16_t strLen = glcdWriteStringFramed(menuGetValueStr(menuIdx), 1);
    glcdSetFontColor(color);
    glcdSetFontBgColor(bgColor);

    // Fill space between name and value
    glcdDrawRect(x, y_pos + 2, width - 2 - x - strLen, fIh, canvas.color);
}

static void canvasImproveSpectrum(SpChan *chan, uint16_t height)
{
    for (uint8_t i = 0; i < FFT_SIZE / 2; i++) {
        chan->raw[i] = height * chan->raw[i] / N_DB;

        chan->old_show[i] = chan->show[i];
        if (chan->raw[i] < chan->show[i]) {
            if (chan->show[i] >= chan->fall[i]) {
                chan->show[i] -= chan->fall[i];
                chan->fall[i]++;
            } else {
                chan->show[i] = 0;
            }
        }

        if (chan->raw[i] > chan->show[i]) {
            chan->show[i] = chan->raw[i];
            chan->fall[i] = 1;
        }

        chan->old_peak[i] = chan->peak[i];
        if (chan->peak[i] <= chan->raw[i]) {
            chan->peak[i] = chan->raw[i] + 1;
        } else {
            if (chan->peak[i]) {
                chan->peak[i]--;
            }
        }
    }
}

static void canvasDrawSpectrumColumn(bool redraw, int16_t x, int16_t y, int16_t w, int16_t h,
                                     int16_t s, int16_t os, int16_t p, int16_t op)
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

    if (redraw) {
        glcdDrawRect(x, y + h - s, w, s, COLOR_SPECTRUM_COLUMN);

        if (p > s) {
            glcdDrawRect(x, y + h - p, w, 1, COLOR_SPECTRUM_PEAK);
        }
        return;
    }

    if (s > os) {
        glcdDrawRect(x, y + h - s, w, s - os, COLOR_SPECTRUM_COLUMN);

    } else if (s < os) {
        glcdDrawRect(x, y + h - os, w, os - s, canvas.color);
    }

    if (p > s) {
        glcdDrawRect(x, y + h - p, w, 1, COLOR_SPECTRUM_PEAK);
    }
    if (op > p && op > s) {
        glcdDrawRect(x, y + h - op, w, 1, canvas.color);
    }

}

static void canvasDrawSpectrumChan(int16_t chan, Spectrum *sp, int16_t y, int16_t height)
{
    const uint8_t step = canvas.lt->sp.step;
    const uint8_t oft = canvas.lt->sp.oft;
    const uint8_t width = canvas.lt->sp.width;

    const int16_t num = (canvas.lt->width + width - 1) / step;    // Number of spectrum columns

    canvasImproveSpectrum(&sp->chan[chan], (uint16_t)canvas.lt->height / 2);

    uint8_t *show = sp->chan[chan].show;
    uint8_t *peak = sp->chan[chan].peak;
    uint8_t *old_show = sp->chan[chan].old_show;
    uint8_t *old_peak = sp->chan[chan].old_peak;

    for (int16_t col = 0; col < num; col++) {
        int16_t x = oft + col * step;
        canvasDrawSpectrumColumn(sp->redraw, x, y, width, height,
                                 *show++, *old_show++, *peak++, *old_peak++);
    }
}

void canvasShowTime(bool clear, RTC_type *rtc)
{
    (void)clear;

    int16_t zeroPos;
    int16_t ltspPos;
    uint16_t timeLen;

    // HH:MM:SS
    glcdSetFont(canvas.lt->time.hmsFont);
    zeroPos = glcdFontSymbolPos('0');
    ltspPos = glcdFontSymbolPos(LETTER_SPACE_CHAR);
    timeLen = 6 * (canvas.lt->time.hmsFont->chars[zeroPos].image->width);    // 6 digits HHMMSS
    timeLen += 15 *
               (canvas.lt->time.hmsFont->chars[ltspPos].image->width);  // 13 letter spaces + 2 ':'
    glcdSetXY((canvas.lt->width - timeLen) / 2, canvas.lt->time.hmsY);

    canvasDrawTm(rtc, RTC_HOUR);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar(':');
    glcdWriteChar(LETTER_SPACE_CHAR);
    canvasDrawTm(rtc, RTC_MIN);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar(':');
    glcdWriteChar(LETTER_SPACE_CHAR);
    canvasDrawTm(rtc, RTC_SEC);

    // DD:MM:YYYY
    glcdSetFont(canvas.lt->time.dmyFont);
    zeroPos = glcdFontSymbolPos('0');
    ltspPos = glcdFontSymbolPos(LETTER_SPACE_CHAR);
    timeLen = 8 * (canvas.lt->time.dmyFont->chars[zeroPos].image->width);    // 8 digits HHMMSS
    timeLen += 17 *
               (canvas.lt->time.dmyFont->chars[ltspPos].image->width);  // 15 letter spaces + 2 '.'
    glcdSetXY((canvas.lt->width - timeLen) / 2, canvas.lt->time.dmyY);

    canvasDrawTm(rtc, RTC_DATE);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    canvasDrawTm(rtc, RTC_MONTH);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    canvasDrawTm(rtc, RTC_YEAR);

    // Weekday
    glcdSetFont(canvas.lt->time.wdFont);
    glcdSetFontColor(LCD_COLOR_AQUA);

    static int8_t wdayOld = 0;
    int8_t wday = rtc->wday;
    if (wday != wdayOld)    // Clear the area with weekday label
        glcdDrawRect(0, canvas.lt->time.wdY, canvas.lt->width,
                     (int16_t)canvas.lt->time.wdFont->chars[0].image->height, canvas.color);
    wdayOld = wday;

    const char **txtLabels = labelsGet();
    const char *wdayLabel = txtLabels[LABEL_SUNDAY + wday];

    glcdSetXY(canvas.lt->width / 2, canvas.lt->time.wdY);
    glcdSetFontAlign(FONT_ALIGN_CENTER);
    glcdWriteString((char *)wdayLabel);
}

void canvasShowMenu(void)
{
    Menu *menu = menuGet();

    const int16_t fHh = (int16_t)canvas.lt->menu.headFont->chars[0].image->height;
    const int16_t fIh = (int16_t)canvas.lt->menu.menuFont->chars[0].image->height;
    const uint8_t items = menu->dispSize;

    const int16_t dividerPos = (canvas.lt->height - (fIh + 4) * items + fHh) / 2;

    // Show header
    char *parentName = menuGetName(menu->parent);
    glcdSetFont(canvas.lt->menu.headFont);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString(parentName);
    // Fill free space after header
    glcdDrawRect(canvas.glcd->x, canvas.glcd->y, canvas.lt->width - canvas.glcd->x, fHh, canvas.color);

    glcdDrawRect(0, dividerPos, canvas.lt->width, 1, canvas.glcd->font.color);

    for (uint8_t idx = 0; idx < menu->listSize; idx++) {
        if (idx >= menu->dispOft && idx < items + menu->dispOft) {
            canvasDrawMenuItem(idx, canvas.lt->menu.menuFont);
        }
    }
}

void canvasShowTune(bool clear, DispParam *dp, Spectrum *sp)
{
    const tFont *iconSet = canvas.lt->iconSet;
    static int16_t valueOld;

    if (clear) {
        // Label
        glcdSetFont(canvas.lt->lblFont);
        glcdSetFontColor(LCD_COLOR_WHITE);
        glcdSetXY(0, 0);
        glcdWriteString((char *)dp->label);
        // Icon
        glcdSetXY(canvas.lt->width - iconSet->chars[0].image->width, 0);
        glcdWriteIcon(dp->icon, iconSet, canvas.lt->iconColor, canvas.color);
    }
    if (clear || valueOld != dp->value) {
        // Bar
        canvasDrawBar(&canvas.lt->tuner.bar, dp->value, dp->min, dp->max);
        // Value
        glcdSetXY(canvas.lt->width, canvas.lt->tune.valY);
        glcdSetFontAlign(FONT_ALIGN_RIGHT);
        glcdSetFont(canvas.lt->tune.valFont);
        glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);
        glcdDrawLine(0, canvas.lt->height / 2, canvas.lt->width - 1, canvas.lt->height / 2,
                     LCD_COLOR_WHITE);
    }
    valueOld = dp->value;

    // Spectrum
    if (!sp->ready) {
        return;
    }

    const int16_t chan = SP_CHAN_LEFT;
    const int16_t height = canvas.lt->height / 2;

    int16_t y = canvas.lt->height / 2;

    canvasDrawSpectrumChan(chan, sp, y, height);

    sp->redraw = false;
    sp->ready = false;
}

void canvasShowSpectrum(bool clear, Spectrum *sp)
{
    (void)clear;

    if (!sp->ready) {
        return;
    }

    const int16_t height = canvas.lt->height / 2;

    for (uint8_t chan = SP_CHAN_LEFT; chan < SP_CHAN_END; chan++) {
        int16_t y = chan * height;
        canvasDrawSpectrumChan(chan, sp, y, height);
    }

    sp->redraw = false;
    sp->ready = false;
}

void canvasShowTuner(Tuner *tuner)
{
    const tFont *fmFont = canvas.lt->lblFont;
    const tFont *iconSet = canvas.lt->iconSet;

    int16_t freq = (int16_t)tuner->freq;
    int16_t freqMin = (int16_t)tuner->par.fMin;
    int16_t freqMax = (int16_t)tuner->par.fMax;

    glcdSetFont(fmFont);
    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetXY(2, 0);

    glcdWriteString("FM ");

    canvasDrawBar(&canvas.lt->tuner.bar, freq, freqMin, freqMax);

    glcdWriteNum(freq / 100, 3, ' ', 10);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteNum(freq % 100, 2, '0', 10);

    // Icon
    glcdSetXY(canvas.lt->width - iconSet->chars[0].image->width, 0);
    glcdWriteIcon(ICON_TUNER, iconSet, canvas.lt->iconColor, canvas.color);

}
