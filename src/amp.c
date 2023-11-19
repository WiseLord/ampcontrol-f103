#include "amp.h"

#include <stdlib.h>
#include <string.h>

#include "audio/audio.h"
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
        utilEnableSwd(ALLOW_SWD && (SCREEN_STANDBY == amp.screen));

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
    // Parsing date format YYYY-MM-DDTHH:MM:SS
    if (strlen(date) >= 19 &&
        date[4] == '-' &&
        date[7] == '-' &&
        date[10] == 'T' &&
        date[13] == ':' &&
        date[16] == ':') {

        RTC_type rtc;

        rtc.year = strtol(date + 0, NULL, 10) % 100;
        rtc.month = strtol(date + 5, NULL, 10);
        rtc.date = strtol(date + 8, NULL, 10);
        rtc.hour = strtol(date + 11, NULL, 10);
        rtc.min = strtol(date + 14, NULL, 10);
        rtc.sec = strtol(date + 17, NULL, 10);

        rtcUpdateTime(&rtc);
    }
}
