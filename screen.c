#include "screen.h"

#include "display/gdfb.h"
#include "rtc.h"

char strbuf[STR_BUFSIZE + 1];   // String buffer

const char STR_SPDOTSP[]        = "\x7F.\x7F";
const char STR_SPCOLSP[]        = "\x7F:\x7F";
const char STR_YEAR20[]         = "20";
const char STR_SP[]             = "\x7F";

static void writeStringPgm(const char *string)
{
//    strcpy_P(strbuf, string);
//    writeString(strbuf);
    writeString((char *)string);
}

static void writeNum(int16_t number, uint8_t width, uint8_t lead, uint8_t radix)
{
    uint8_t numdiv;
    uint8_t sign = lead;
    int8_t i;

    if (number < 0) {
        sign = '-';
        number = -number;
    }

    for (i = 0; i < width; i++)
        strbuf[i] = lead;
    strbuf[width] = '\0';
    i = width - 1;

    while (number > 0 || i == width - 1) {
        numdiv = number % radix;
        strbuf[i] = numdiv + 0x30;
        if (numdiv >= 10)
            strbuf[i] += 7;
        i--;
        number /= radix;
    }

    if (i >= 0)
        strbuf[i] = sign;

    writeString(strbuf);
}

static void drawTm(RTC_type *rtc, uint8_t tm, const uint8_t *font)
{
    gdLoadFont(font, rtc->etm == tm ? 0 : 1, FONT_DIR_0);
    writeNum(*((int8_t *)rtc + tm), 2, '0', 10);
    gdLoadFont(font, 1, FONT_DIR_0);
}

void screenTime(int8_t etm)
{
    RTC_type rtc;
    rtcGetTime(&rtc);
    rtc.etm = etm;

    uint32_t sec = rtcToSec(&rtc);
    secToRtc(sec, &rtc);

    gdSetXY(4, 0);

    drawTm(&rtc, RTC_HOUR, font_digits_32);
    writeStringPgm(STR_SPCOLSP);
    drawTm(&rtc, RTC_MIN, font_digits_32);
    writeStringPgm(STR_SPCOLSP);
    drawTm(&rtc, RTC_SEC, font_digits_32);

    gdSetXY(9, 32);

    drawTm(&rtc, RTC_DATE, font_ks0066_ru_24);
    writeStringPgm(STR_SPDOTSP);
    drawTm(&rtc, RTC_MONTH, font_ks0066_ru_24);
    writeStringPgm(STR_SPDOTSP);
    if (rtc.etm == RTC_YEAR)
        gdLoadFont(font_ks0066_ru_24, 0, FONT_DIR_0);
    writeStringPgm(STR_YEAR20);
    writeStringPgm(STR_SP);
    drawTm(&rtc, RTC_YEAR, font_ks0066_ru_24);

    gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
    gdSetXY(32, 56);

    writeNum(rtc.wday, 1, ' ', 10);
//    writeStringEeprom(txtLabels[LABEL_SUNDAY + (rtcWeekDay() - 1) % 7]);
}


void screenNum(int16_t num)
{
    writeNum(num, 5, ' ', 10);
}
