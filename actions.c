#include "actions.h"

#include "input.h"
#include "rtc.h"
#include "screen.h"
#include "swtimers.h"

static Screen screen = SCREEN_STANDBY;
static Screen screenDefault = SCREEN_SPECTRUM;

static RtcMode rtcMode = RTC_NOEDIT;
static Action action = {ATYPE_NONE, 0};

static void actionSet(uint8_t type, uint8_t mode, int16_t value)
{
    action.type = type;
    action.mode = mode;
    action.value = value;
}

static void actionHandleButtons()
{
    CmdBtn cmdBtn = getBtnCmd();

    if (cmdBtn & 0xFF00) {
        cmdBtn >>= 8;
        action.mode = AMODE_BTN_LONG;
    }

    switch (cmdBtn) {
    case BTN_D0:
        action.type = ATYPE_BTN0;
        break;
    case BTN_D1:
        action.type = ATYPE_BTN1;
        break;
    case BTN_D2:
        action.type = ATYPE_BTN2;
        break;
    case BTN_D3:
        action.type = ATYPE_BTN3;
        break;
    case BTN_D4:
        action.type = ATYPE_BTN4;
        break;
    case BTN_D5:
        action.type = ATYPE_BTN5;
        break;
    }
}

static void actionRemapButtons()
{
    switch (action.type) {
    case ATYPE_BTN0:
        switch (action.mode) {
        case AMODE_BTN_SHORT:
            action.type = ATYPE_STANDBY;
            action.mode = AMODE_STANDBY_SWITCH;
            break;
        }
        break;
    case ATYPE_BTN2:
        switch (action.mode) {
        case AMODE_BTN_SHORT:
            action.type = ATYPE_RTC;
            switch (screen) {
            case SCREEN_TIME:
                action.mode = AMODE_RTC_MODE;
                break;
            default:
                action.mode = AMODE_RTC_SHOW;
                break;
            }
            break;
        }
        break;
    case ATYPE_BTN4:
        switch (action.mode) {
        case AMODE_BTN_SHORT:
            if (screen == SCREEN_TIME && rtcMode != RTC_NOEDIT) {
                actionSet(ATYPE_RTC, AMODE_RTC_CHANGE, -1);
            }
        }
        break;
    case ATYPE_BTN5:
        switch (action.mode) {
        case AMODE_BTN_SHORT:
            if (screen == SCREEN_TIME && rtcMode != RTC_NOEDIT) {
                actionSet(ATYPE_RTC, AMODE_RTC_CHANGE, +1);
            }
        }
        break;
    }
}

static void actionRemapActions()
{
    switch (action.type) {
    case ATYPE_STANDBY:
        if (action.mode == AMODE_STANDBY_SWITCH) {
            switch (screen) {
            case SCREEN_STANDBY:
                action.mode = AMODE_STANDBY_EXIT;
                break;
            default:
                action.mode = AMODE_STANDBY_ENTER;
                break;
            }
        }
        break;
    default:
        break;
    }
}

static void actionHandleTimers()
{
    if (action.type == ATYPE_NONE) {
        if (swTimGetDisplay() == 0) {
            action.type = ATYPE_DISPTIME;
            swTimSetDisplay(SW_TIM_OFF);
        }
    }
}

static void actionHandleEncoder()
{
    if (action.type == ATYPE_NONE) {
        int8_t encCnt = getEncoder();

        if (encCnt) {
            switch (screen) {
            case SCREEN_TIME:
                actionSet(ATYPE_RTC, AMODE_RTC_CHANGE, encCnt);
                break;
            default:
                break;
            }
        }
    }
}

void actionGet()
{
    actionSet(ATYPE_NONE, 0, 0);

    actionHandleButtons();
    actionRemapButtons();
    actionRemapActions();

    actionHandleEncoder();

    actionHandleTimers();
}

void actionHandle()
{
    switch (action.type) {
    case ATYPE_STANDBY:
        switch (action.mode) {
        case AMODE_STANDBY_ENTER:
            screen = SCREEN_STANDBY;
            rtcMode = RTC_NOEDIT;
            gdSetBrightness(GD_MAX_BRIGHTNESS / 8);
            break;
        case AMODE_STANDBY_EXIT:
            screen = SCREEN_TIME;
            gdSetBrightness(GD_MAX_BRIGHTNESS);
            swTimSetDisplay(1000);
            break;
        }
        break;
    case ATYPE_RTC:
        screen = SCREEN_TIME;
        switch (action.mode) {
        case AMODE_RTC_SHOW:
            swTimSetDisplay(5000);
            break;
        case AMODE_RTC_MODE:
            swTimSetDisplay(5000);
            if (++rtcMode > RTC_NOEDIT) {
                rtcMode = RTC_HOUR;
            }
            break;
        case AMODE_RTC_CHANGE:
            swTimSetDisplay(5000);
            rtcChangeTime(rtcMode, action.value);
            break;
        }
        break;
    case ATYPE_DISPTIME:
        rtcMode = RTC_NOEDIT;
        switch (screen) {
        case SCREEN_STANDBY:
            break;
        default:
            screen = screenDefault;
            break;
        }
        break;
    }
}

void actionShowScreen()
{
    static Screen screenPrev = SCREEN_STANDBY;

    // Clear display if screen mode has changed
    if (screen != screenPrev) {
        gdClear();
    }

    switch (screen) {
    case SCREEN_STANDBY:
    case SCREEN_TIME:
        screenTime(rtcMode);
        break;
    case SCREEN_SPECTRUM:
        screenSpectrum();
        break;
    default:
        break;
    }

    // Save current screen as previous
    screenPrev = screen;
}
