#include "actions.h"

#include "input.h"
#include "screen.h"
#include "rtc.h"

static Screen screen = SCREEN_STANDBY;
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

static Action actionRemap(Action action)
{
    switch (action) {
    case ACTION_BTN0:
        switch (screen) {
        case SCREEN_STANDBY:
            action = ACTION_EXIT_STANDBY;
            break;
        default:
            action = ACTION_GO_STANDBY;
            break;
        }
        break;
    case ACTION_BTN2:
        switch (screen) {
        case SCREEN_TIME:
            action = ACTION_NEXT_RTCMODE;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    return action;
}

Action actionGet()
{
    Action action = ACTION_NONE;

    action = actionHandleButtons();

    action = actionRemap(action);

    return action;
}

void actionHandle(Action action)
{
    switch (action) {
    case ACTION_GO_STANDBY:
        screen = SCREEN_STANDBY;
        rtcMode = RTC_NOEDIT;
        gdSetBrightness(GD_MAX_BRIGHTNESS / 8);
        break;
    case ACTION_EXIT_STANDBY:
        screen = SCREEN_TIME;
        gdSetBrightness(GD_MAX_BRIGHTNESS);
        break;
    case ACTION_NEXT_RTCMODE:
        screen = SCREEN_TIME;
        if (++rtcMode > RTC_NOEDIT) {
            rtcMode = RTC_HOUR;
        }
        break;
    default:
        break;
    }
}

void actionShowScreen()
{
    switch (screen) {
    case SCREEN_STANDBY:
    case SCREEN_TIME:
        screenTime(rtcMode);
        break;
    default:
        break;
    }
}
