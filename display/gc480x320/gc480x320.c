#include "../dispdrv.h"

static GlcdDriver *glcd;

static void displayTm(RTC_type *rtc, uint8_t tm)
{
    int8_t time = *((int8_t *)rtc + tm);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdWriteChar(LETTER_SPACE_CHAR);
    if (rtc->etm == tm)
        glcdSetFontColor(LCD_COLOR_OLIVE);
    if (tm == RTC_YEAR) {
        glcdWriteString("20");
        glcdWriteChar(LETTER_SPACE_CHAR);
    }
    glcdWriteNum(time, 2, '0', 10);
    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdWriteChar(LETTER_SPACE_CHAR);
}

static void drawShowBar(int16_t value, int16_t min, int16_t max)
{
    static const int16_t sc = 80; // Scale count
    static const uint8_t sw = 3; // Scale width

    if (min + max) { // Non-symmectic scale => rescale to 0..sl
        value = sc * (value - min) / (max - min);
    } else { // Symmetric scale => rescale to -sl/2..sl/2
        value = (sc / 2) * value / max;
    }

    for (uint16_t i = 0; i < sc; i++) {
        uint16_t color = LCD_COLOR_WHITE;

        if (min + max) { // Non-symmetric scale
            if (i >= value) {
                color = glcd->canvas->color;
            }
        } else { // Symmetric scale
            if ((value > 0 && i >= value + (sc / 2)) ||
                (value >= 0 && i < (sc / 2 - 1)) ||
                (value < 0 && i < value + (sc / 2)) ||
                (value <= 0 && i > (sc / 2))) {
                color = glcd->canvas->color;
            }
        }

        glcdDrawRect(i * (glcd->canvas->width / sc) + 1, 100, sw, 14, color);
        glcdDrawRect(i * (glcd->canvas->width / sc) + 1, 114, sw, 2, LCD_COLOR_WHITE);
        glcdDrawRect(i * (glcd->canvas->width / sc) + 1, 116, sw, 14, color);
    }
}

static void drawSpCol(uint16_t xbase, uint16_t ybase, uint8_t width, uint8_t value, uint8_t max, uint8_t peak)
{
    if (value > max)
        value = max;

    if (peak > max - 1)
        peak = max - 1;

    glcdDrawRect(xbase, ybase - value, width, value, LCD_COLOR_AQUA);
    glcdDrawRect(xbase, ybase - max, width, max - value, LCD_COLOR_BLACK);
    glcdDrawRect(xbase, ybase - peak - 1, width, 1, LCD_COLOR_YELLOW);
}

static void showTime(RTC_type *rtc, char *wday)
{
    glcdSetXY(2, 10);
    glcdSetFont(&fontterminusdig120);

    displayTm(rtc, RTC_HOUR);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar(':');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_MIN);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar(':');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_SEC);

    glcdSetXY(20, 130);
    glcdSetFont(&fontterminusdig96);

    displayTm(rtc, RTC_DATE);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_MONTH);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_YEAR);

    glcdSetXY(240, 224);
    glcdSetFont(&fontterminusmod96);
    glcdSetFontColor(LCD_COLOR_AQUA);

    static char *wdayOld = 0;
    if (wday != wdayOld) {
        glcdDrawRect(0, 224, 480, 9, glcd->canvas->color);
    }

    glcdSetFontAlign(FONT_ALIGN_CENTER);
    glcdWriteString(wday);

    wdayOld = wday;
}

static void showParam(DispParam *dp)
{
    glcdSetFont(&fontterminusmod96);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString((char *)dp->label);

    drawShowBar(dp->value, dp->min, dp->max);

    glcdSetXY(471, 200);
    glcdSetFont(&fontterminusdig120);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);
}

static void showSpectrum(SpectrumData *spData)
{
    uint8_t *buf;
    uint8_t *peak;

    buf = spData[SP_CHAN_LEFT].show;
    peak = spData[SP_CHAN_LEFT].peak;
    for (uint16_t x = 0; x < (glcd->canvas->width + 1) / 4; x++) {
        uint16_t xbase = x * 4;
        uint16_t ybase = 160;
        uint16_t width = 2;
        uint8_t value = buf[x];
        uint8_t pValue = peak[x];
        uint8_t max = 159;

        drawSpCol(xbase, ybase, width, value + 1, max, pValue);
    }

    buf = spData[SP_CHAN_RIGHT].show;
    peak = spData[SP_CHAN_RIGHT].peak;
    for (uint16_t x = 0; x < (glcd->canvas->width + 1) / 4; x++) {
        uint16_t xbase = x * 4;
        uint16_t ybase = 320;
        uint16_t width = 2;
        uint8_t value = buf[x];
        uint8_t pValue = peak[x];
        uint8_t max = 159;

        drawSpCol(xbase, ybase, width, value + 1, max, pValue);
    }
}

GlcdCanvas gc480x320 = {
    .width = 480,
    .height = 320,

    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
};

void gc480x320Init(GlcdDriver *driver)
{
    glcd = driver;
    glcd->canvas = &gc480x320;
    glcdInit(glcd);
}
