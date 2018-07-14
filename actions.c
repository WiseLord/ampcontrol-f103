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
    Action action = {ACTION_NONE, ACTVAL_BTN_SHORT};

    CmdBtn cmdBtn = getBtnCmd();

    if (cmdBtn & 0xFF00) {
        cmdBtn >>= 8;
        action.value = ACTVAL_BTN_LONG;
    }

    switch (cmdBtn) {
    case BTN_D0:
        action.param = ACTION_BTN0;
        break;
    case BTN_D1:
        action.param = ACTION_BTN1;
        break;
    case BTN_D2:
        action.param = ACTION_BTN2;
        break;
    case BTN_D3:
        action.param = ACTION_BTN3;
        break;
    case BTN_D4:
        action.param = ACTION_BTN4;
        break;
    case BTN_D5:
        action.param = ACTION_BTN5;
        break;
    }

    return action;
}

static Action actionRemapButtons(Action action)
{
    switch (action.param) {
    case ACTION_BTN0:
        switch (action.value) {
        case ACTVAL_BTN_SHORT:
            action.param = ACTION_STANDBY;
            action.value = ACTVAL_STANDBY_SWITCH;
            break;
        }
        break;
    case ACTION_BTN2:
        switch (action.value) {
        case ACTVAL_BTN_SHORT:
            action.param = ACTION_RTC;
            switch (screen) {
            case SCREEN_TIME:
                action.value = ACTVAL_RTC_EDIT;
                break;
            default:
                action.value = ACTVAL_RTC_SHOW;
                break;
            }
            break;
        }
        break;
    }

    return action;
}

static Action actionRemapActions(Action action)
{
    switch (action.param) {
    case ACTION_STANDBY:
        if (action.value == ACTVAL_STANDBY_SWITCH) {
            switch (screen) {
            case SCREEN_STANDBY:
                action.value = ACTVAL_STANDBY_EXIT;
                break;
            default:
                action.value = ACTVAL_STANDBY_ENTER;
                break;
            }
        }
        break;
    default:
        break;
    }

    return action;
}

static Action actionHandleTimers()
{
    Action action = {ACTION_NONE, 0};

    if (swTimGetDisplay() == 0) {
        action.param = ACTION_DISPTIME;
        swTimSetDisplay(SW_TIM_OFF);
    }

    return action;
}

Action actionGet()
{
    Action action = {ACTION_NONE, 0};

    action = actionHandleButtons();
    action = actionRemapButtons(action);
    action = actionRemapActions(action);

    if (action.param == ACTION_NONE) {
        action = actionHandleTimers();
    }

    return action;
}

void actionHandle(Action action)
{
    switch (action.param) {
    case ACTION_STANDBY:
        switch (action.value) {
        case ACTVAL_STANDBY_ENTER:
            screen = SCREEN_STANDBY;
            rtcMode = RTC_NOEDIT;
            gdSetBrightness(GD_MAX_BRIGHTNESS / 8);
            break;
        case ACTVAL_STANDBY_EXIT:
            screen = SCREEN_TIME;
            gdSetBrightness(GD_MAX_BRIGHTNESS);
            swTimSetDisplay(1000);
            break;
        }
        break;
    case ACTION_RTC:
        switch (action.value) {
        case ACTVAL_RTC_SHOW:
            screen = SCREEN_TIME;
            swTimSetDisplay(5000);
            break;
        case ACTVAL_RTC_EDIT:
            screen = SCREEN_TIME;
            swTimSetDisplay(5000);
            if (++rtcMode > RTC_NOEDIT) {
                rtcMode = RTC_HOUR;
            }
            break;
        }
        break;
    case ACTION_DISPTIME:
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
