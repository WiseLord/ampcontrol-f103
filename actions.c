#include "actions.h"

#include "input.h"
#include "rtc.h"
#include "screen.h"
#include "swtimers.h"

static Screen screen = SCREEN_STANDBY;
static Screen screenDefault = SCREEN_SPECTRUM;

static RtcMode rtcMode = RTC_NOEDIT;

static Action actionHandleButtons()
{
    Action action = ACTION_NONE;

    CmdBtn cmdBtn = getBtnCmd();
    switch (cmdBtn) {
    case BTN_D0:
        action = ACTION_BTN0;
        break;
    case BTN_D1:
        action = ACTION_BTN1;
        break;
    case BTN_D2:
        action = ACTION_BTN2;
        break;
    case BTN_D3:
        action = ACTION_BTN3;
        break;
    case BTN_D4:
        action = ACTION_BTN4;
        break;
    case BTN_D5:
        action = ACTION_BTN5;
        break;
    case BTN_D0 << 8:
        action = ACTION_BTN0_LONG;
        break;
    case BTN_D1 << 8:
        action = ACTION_BTN1_LONG;
        break;
    case BTN_D2 << 8:
        action = ACTION_BTN2_LONG;
        break;
    case BTN_D3 << 8:
        action = ACTION_BTN3_LONG;
        break;
    case BTN_D4 << 8:
        action = ACTION_BTN4_LONG;
        break;
    case BTN_D5 << 8:
        action = ACTION_BTN5_LONG;
        break;
    default:
        action = ACTION_NONE;
        break;
    }

    return action;
}

static Action actionRemapButtons(Action action)
{
    switch (action) {
    case ACTION_BTN0:
        action = ACTION_STANDBY;
        break;
    case ACTION_BTN2:
        switch (screen) {
        case SCREEN_TIME:
            action = ACTION_RTC_EDIT;
            break;
        default:
            action = ACTION_RTC;
            break;
        }
        break;
    default:
        break;
    }

    return action;
}

static Action actionRemapActions(Action action)
{
    switch (action) {
    case ACTION_STANDBY:
        switch (screen) {
        case SCREEN_STANDBY:
            action = ACTION_STANDBY_EXIT;
            break;
        default:
            action = ACTION_STANDBY_ENTER;
            break;
        }
        break;
    default:
        break;
    }

    return action;
}

static Action actionHandleTimers()
{
    Action action = ACTION_NONE;

    if (swTimGetDisplay() == 0) {
        action = ACTION_DISTIME_EXPIRED;
        swTimSetDisplay(SW_TIM_OFF);
    }

    return action;
}

Action actionGet()
{
    Action action = ACTION_NONE;

    action = actionHandleButtons();
    action = actionRemapButtons(action);
    action = actionRemapActions(action);

    if (action == ACTION_NONE) {
        action = actionHandleTimers();
    }

    return action;
}

void actionHandle(Action action)
{
    switch (action) {
    case ACTION_STANDBY_ENTER:
        screen = SCREEN_STANDBY;
        rtcMode = RTC_NOEDIT;
        gdSetBrightness(GD_MAX_BRIGHTNESS / 8);
        break;
    case ACTION_STANDBY_EXIT:
        screen = SCREEN_TIME;
        gdSetBrightness(GD_MAX_BRIGHTNESS);
        swTimSetDisplay(1000);
        break;
    case ACTION_RTC:
        screen = SCREEN_TIME;
        swTimSetDisplay(5000);
        break;
    case ACTION_RTC_EDIT:
        screen = SCREEN_TIME;
        swTimSetDisplay(5000);
        if (++rtcMode > RTC_NOEDIT) {
            rtcMode = RTC_HOUR;
        }
        break;
    case ACTION_DISTIME_EXPIRED:
        switch (screen) {
        case SCREEN_STANDBY:
            break;
        default:
            screen = screenDefault;
            break;
        }
        break;
    default:
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
