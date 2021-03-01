#include "amp.h"

#include <stdio.h>
#include <string.h>

#include "input.h"
#include "rtc.h"
#include "utils.h"

static Amp amp = {
    .status = AMP_STATUS_STBY,
    .screen = SCREEN_STANDBY,
    .defScreen = SCREEN_SPECTRUM,
};

Amp *ampGet(void)
{
    return &amp;
}

void ampRun(void)
{
    while (1) {
        utilEnableSwd(SCREEN_STANDBY == amp.screen);

        ampSyncFromOthers();

        ampActionGet();
        ampActionRemap();
        ampActionHandle();

        ampSyncToOthers();

        ampScreenShow();
    }
}

Action ampGetButtons()
{
    Action ret = {.type = ACTION_NONE, .value = 0};

    CmdBtn cmdBtn = inputGetBtnCmd();

    if (cmdBtn.btn) {
        ret.value = cmdBtn.btn;
        if (cmdBtn.flags & BTN_FLAG_LONG_PRESS) {
            ret.type = ACTION_BTN_LONG;
        } else {
            ret.type = ACTION_BTN_SHORT;
        }
    }

    return ret;
}

Action ampGetEncoder(void)
{
    Action ret = {.type = ACTION_NONE, .value = 0};

    ret.value = inputGetEncoder();

    if (ret.value) {
        ret.type = ACTION_ENCODER;
    }

    return ret;
}

void ampUpdateDate(char *date)
{
    int year, month, day, hour, min, sec;

    int ret = sscanf(date, "%04d-%02d-%02dT%02d:%02d:%02d",
                     &year, &month, &day, &hour, &min, &sec);

    RTC_type rtc;

    if (ret == 6) {
        rtc.year = year % 100;
        rtc.month = month;
        rtc.date = day;
        rtc.hour = hour;
        rtc.min = min;
        rtc.sec = sec;
    }

    rtcUpdateTime(&rtc);
}
