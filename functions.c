#include "functions.h"

#include "display/gdfb.h"
#include "rtc.h"
#include <stm32f1xx_ll_rtc.h>

void _delay_us(uint32_t us)
{
    volatile uint32_t n = 8 * us;

    while (--n > 0);
}

void _delay_ms(uint32_t ms)
{
    volatile uint32_t n = 1000;

    while (--n > 0) {
        _delay_us(ms);
    }

}

void _show_number(uint32_t number)
{
    char buf[] = "         0";
    uint8_t pos = 9;
    while (number) {
        buf[pos--] = '0' + number % 10;
        number /= 10;
    }

    gdWriteString(buf);
}

void _show_time()
{
    gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
    gdSetXY(5, 0);

    uint32_t time = LL_RTC_TIME_Get(RTC);
    _show_number(time);

    RTC_type rtc;

    secToRtc(time, &rtc);

    gdSetXY(0, 8);
    _show_number(rtc.hour);
    gdSetXY(0, 16);
    _show_number(rtc.min);
    gdSetXY(0, 24);
    _show_number(rtc.sec);

    gdSetXY(64, 8);
    _show_number(rtc.date);
    gdSetXY(64, 16);
    _show_number(rtc.month);
    gdSetXY(64, 24);
    _show_number(rtc.year);
}
