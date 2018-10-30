#include "canvas.h"

// On 220x176 we can draw max 8 menu items + menu header
#define MENU_SIZE_VISIBLE   7

static void showTime(RTC_type *rtc, char *wday);
static void showParam(DispParam *dp);
static void showSpectrum(bool clear, SpectrumData *spData);
//static void showTuner(DispTuner *dt);
static void showMenu(void);

static Canvas canvas = {
    .width = 220,
    .height = 176,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    //.showTuner = showTuner,
    .showMenu = showMenu,
};

void gc220x176Init(Canvas **value)
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

static void showTime(RTC_type *rtc, char *wday)
{
    glcdSetXY(6, 10);
    glcdSetFont(&fontterminusdig58);

    displayTm(rtc, RTC_HOUR);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar(':');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_MIN);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar(':');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_SEC);

    glcdSetXY(10, 78);
    glcdSetFont(&fontterminusdig40);

    displayTm(rtc, RTC_DATE);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_MONTH);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_YEAR);

    glcdSetXY(109, 126);
    glcdSetFont(&fontterminusmod44);
    glcdSetFontColor(LCD_COLOR_AQUA);

    static char *wdayOld = 0;
    if (wday != wdayOld) {
        glcdDrawRect(0, 124, 220, 32, canvas.color);
    }

    glcdSetFontAlign(FONT_ALIGN_CENTER);
    glcdWriteString(wday);

    wdayOld = wday;
}

static void showParam(DispParam *dp)
{
    glcdSetFont(&fontterminusmod44);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString((char *)dp->label);

    glcdSetXY(217, 118);
    glcdSetFont(&fontterminusdig58);

    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);
}

static void showSpectrum(bool clear, SpectrumData *spData)
{
    const uint8_t step = 3;     // Step in pixels between spectrum columns
    const uint8_t oft = 1;      // Offset of spectrum column inside step

    const uint8_t width = 2;    // Width of spectrum column

    canvasShowSpectrum(clear, spData, step, oft, width);
}

static void showMenu(void)
{
    canvasShowMenu(&fontterminus20b, &fontterminus18);
}
