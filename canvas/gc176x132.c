#include "canvas.h"

// On 176x132 we can draw max 5 menu items + menu header
#define MENU_SIZE_VISIBLE   7

static void showTime(RTC_type *rtc, char *wday);
static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
//static void showTuner(DispTuner *dt);
static void showMenu(void);

static Canvas canvas = {
    .width = 176,
    .height = 132,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    //.showTuner = showTuner,
    .showMenu = showMenu,
};

void gc176x132Init(Canvas **value)
{
    *value = &canvas;
    menuGet()->dispSize = MENU_SIZE_VISIBLE;
}

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

static void displayShowBar(int16_t min, int16_t max, int16_t value)
{
    static const int16_t sc = 46; // Scale count
    static const uint8_t sw = 1; // Scale width

    if (min + max) { // Non-symmectic scale => rescale to 0..sl
        value = sc * (value - min) / (max - min);
    } else { // Symmetric scale => rescale to -sl/2..sl/2
        value = (sc / 2) * value / max;
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

        uint16_t width = canvas.width;

        glcdDrawRect(i * (width / sc) + 1, 27, sw, 5, color);
        glcdDrawRect(i * (width / sc) + 1, 32, sw, 1, LCD_COLOR_WHITE);
        glcdDrawRect(i * (width / sc) + 1, 33, sw, 5, color);
    }
}

static void showTime(RTC_type *rtc, char *wday)
{
    glcdSetXY(4, 8);
    glcdSetFont(&fontterminusdig42);

    displayTm(rtc, RTC_HOUR);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar(':');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_MIN);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar(':');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_SEC);

    glcdSetXY(8, 58);
    glcdSetFont(&fontterminusdig30);

    displayTm(rtc, RTC_DATE);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_MONTH);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_YEAR);

    glcdSetXY(88, 96);
    glcdSetFont(&fontterminus28);
    glcdSetFontColor(LCD_COLOR_AQUA);

    static char *wdayOld = 0;
    if (wday != wdayOld) {
        glcdDrawRect(0, 96, 176, 32, canvas.color);
    }

    glcdSetFontAlign(FONT_ALIGN_CENTER);
    glcdWriteString(wday);

    wdayOld = wday;
}

static void showParam(DispParam *dp)
{
    glcdSetFont(&fontterminus24b);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(0, 0);
    glcdWriteString((char *)dp->label);

    displayShowBar(dp->min, dp->max, dp->value);

    glcdSetXY(128, 30);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);

    glcdSetXY(104, 2);
    glcdWriteIcon(dp->icon, icons_24, LCD_COLOR_WHITE, canvas.color);
}

static void showSpectrum(bool clear, SpectrumData *spData)
{
    const uint8_t step = 2;     // Step in pixels between spectrum columns
    const uint8_t oft = 0;      // Offset of spectrum column inside step

    const uint8_t width = 1;    // Width of spectrum column

    canvasShowSpectrum(clear, spData, step, oft, width);
}

static void showMenu(void)
{
    canvasShowMenu(&fontterminus16b, &fontterminus12);
}
