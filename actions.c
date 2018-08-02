#include "actions.h"

#include "display.h"
#include "input.h"
#include "rtc.h"
#include "screen.h"
#include "timers.h"
#include "spectrum.h"

static Screen screenDefault = SCREEN_SPECTRUM;

static RtcMode rtcMode = RTC_NOEDIT;
static Action action = {ATYPE_NONE, 0};

static void actionSet(uint8_t type, uint8_t mode, int16_t value)
{
    action.type = type;
    action.mode = mode;
    action.value = value;
}

static void actionHandleButtons(void)
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

static void actionRemapButtons(void)
{
    Screen screen = screenGet();

    switch (action.type) {
    case ATYPE_BTN0:
        switch (action.mode) {
        case AMODE_BTN_SHORT:
            action.type = ATYPE_STANDBY;
            action.mode = AMODE_STANDBY_SWITCH;
            break;
        case AMODE_BTN_LONG:
            action.type = ATYPE_BRIGHTNESS;
            action.mode = AMODE_BRIGHTNESS_WORK;
            break;
        }
        break;
    // For test purposes
    case ATYPE_BTN1:
//        spConvertADC();
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

static void actionRemapActions(void)
{
    switch (action.type) {
    case ATYPE_STANDBY:
        if (action.mode == AMODE_STANDBY_SWITCH) {
            switch (screenGet()) {
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

    // Disable any action except EXIT_STANDBY in standby mode
    if (screenGet() == SCREEN_STANDBY && action.mode != AMODE_STANDBY_EXIT) {
        action.type = ATYPE_NONE;
    }
}

static void actionHandleTimers(void)
{
    if (action.type == ATYPE_NONE) {
        if (swTimGetDisplay() == 0) {
            action.type = ATYPE_DISPTIME;
            swTimSetDisplay(SW_TIM_OFF);
        }
    }
}

static void actionHandleEncoder(void)
{
    if (action.type == ATYPE_NONE) {
        int8_t encCnt = getEncoder();

        if (encCnt) {
            switch (screenGet()) {
            case SCREEN_TIME:
                actionSet(ATYPE_RTC, AMODE_RTC_CHANGE, encCnt);
                break;
            case SCREEN_BRIGHTNESS:
                actionSet(ATYPE_BRIGHTNESS, AMODE_BRIGHTNESS_WORK, encCnt);
                break;
            default:
                break;
            }
        }
    }
}

void actionGet(void)
{
    actionSet(ATYPE_NONE, 0, 0);

    actionHandleButtons();
    actionRemapButtons();
    actionRemapActions();

    actionHandleEncoder();

    actionHandleTimers();
}

void actionHandle(void)
{
    switch (action.type) {
    case ATYPE_STANDBY:
        switch (action.mode) {
        case AMODE_STANDBY_ENTER:
            screenSet(SCREEN_STANDBY);
            rtcMode = RTC_NOEDIT;
            displayChangeBrighness(AMODE_BRIGNTNESS_STANDBY, 0);
            break;
        case AMODE_STANDBY_EXIT:
            screenSet(SCREEN_TIME);
            displayChangeBrighness(AMODE_BRIGHTNESS_WORK, 0);
            swTimSetDisplay(1000);
            break;
        }
        break;
    case ATYPE_RTC:
        screenSet(SCREEN_TIME);
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
        screenSet(RTC_NOEDIT);
        switch (screenGet()) {
        case SCREEN_STANDBY:
            break;
        default:
            screenSet(screenDefault);
            break;
        }
        break;
    case ATYPE_BRIGHTNESS:
        screenSet(SCREEN_BRIGHTNESS);
        swTimSetDisplay(5000);
        displayChangeBrighness(action.mode, action.value);
        break;
    }
}

void actionShowScreen(void)
{
    static Screen screenPrev = SCREEN_STANDBY;
    Screen screen = screenGet();

    // Clear display if screen mode has changed
    if (screen != screenPrev) {
        gdClear();
    }

    switch (screen) {
    case SCREEN_TIME:
        screenTime(rtcMode);
        break;
    case SCREEN_STANDBY:
    case SCREEN_SPECTRUM:
        screenSpectrum();
        break;
    case SCREEN_BRIGHTNESS:
        screenBrightness();
        break;
    default:
        break;
    }

    // Save current screen as previous
    screenPrev = screen;
}
