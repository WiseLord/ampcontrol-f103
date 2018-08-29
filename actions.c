#include "actions.h"

#include "input.h"
#include "rtc.h"
#include "screen.h"
#include "spectrum.h"
#include "timers.h"

static Action action = {ACTION_NONE, 0};

static void actionSet(ActionType type, int16_t value)
{
    action.type = type;
    action.value = value;
}

static void actionHandleButtons(void)
{
    CmdBtn cmdBtn = getBtnCmd();

    if (cmdBtn & 0xFF00) {
        cmdBtn >>= 8;
        action.type = ACTION_BTN_LONG;
    } else if (cmdBtn & 0x00FF) {
        action.type = ACTION_BTN_SHORT;
    }

    action.value = cmdBtn;
}

static void actionRemapButtons(void)
{
    Screen screen = screenGet();

    switch (action.type) {
    case ACTION_BTN_SHORT:
        switch (action.value) {
        case BTN_D0:
            action.type = ACTION_STANDBY;
            action.value = CMD_SWITCH;
            break;
        case BTN_D1:
            break;
        case BTN_D2:
            if (SCREEN_TIME == screen) {
                action.type = ACTION_RTC_MODE;
            } else {
                action.type = ACTION_RTC_SHOW;
            }
            break;
        case BTN_D3:
            break;
        case BTN_D4:
            break;
        case BTN_D5:
            break;
        default:
            break;
        }
        break;
    case ACTION_BTN_LONG:
        switch (action.value) {
        case BTN_D0:
            action.type = ACTION_BR_WORK;
            break;
        case BTN_D1:
            break;
        case BTN_D2:
            break;
        case BTN_D3:
            break;
        case BTN_D4:
            break;
        case BTN_D5:
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

static void actionRemapActions(void)
{
    Screen screen = screenGet();

    switch (action.type) {
    case ACTION_STANDBY:
        if (CMD_SWITCH == action.value) {
            action.value = (SCREEN_STANDBY == screen ? CMD_OFF : CMD_ON);
        }
        break;
    default:
        break;
    }

    if (SCREEN_STANDBY == screen && ACTION_STANDBY != action.type) {
        actionSet(ACTION_NONE, 0);
    }
}

static void actionHandleTimers(void)
{
    if (ACTION_NONE == action.type) {
        if (swTimGetDisplay() == 0) {
            action.type = ACTION_TIMER_EXPIRED;
            swTimSetDisplay(SW_TIM_OFF);
        }
    }
}

static void actionHandleEncoder(void)
{
    Screen screen = screenGet();

    if (ACTION_NONE == action.type) {
        int8_t encCnt = getEncoder();

        if (encCnt) {
            switch (screen) {
            case SCREEN_TIME:
                actionSet(ACTION_RTC_CHANGE, encCnt);
                break;
            case SCREEN_BRIGHTNESS:
                actionSet(ACTION_BR_WORK, encCnt);
                break;
            default:
                break;
            }
        }
    }
}

void actionGet(void)
{
    actionSet(ACTION_NONE, 0);

    actionHandleButtons();
    actionRemapButtons();
    actionRemapActions();

    actionHandleEncoder();

    actionHandleTimers();
}

void actionHandle(void)
{
    Screen screen = screenGet();

    switch (action.type) {
    case ACTION_STANDBY:
        if (action.value == CMD_OFF) {
            screenSet(SCREEN_TIME);
            screenChangeBrighness(ACTION_BR_WORK, 0);
            swTimSetDisplay(1000);
        } else {
            swTimSetDisplay(SW_TIM_OFF);
            screenSet(SCREEN_STANDBY);
            rtcSetMode(RTC_NOEDIT);
            screenChangeBrighness(ACTION_BR_STBY, 0);
        }
        break;

    case ACTION_RTC_SHOW:
        screenSet(SCREEN_TIME);
        swTimSetDisplay(5000);
        break;
    case ACTION_RTC_MODE:
        swTimSetDisplay(15000);
        rtcModeNext();
        break;
    case ACTION_RTC_CHANGE:
        swTimSetDisplay(5000);
        rtcChangeTime(rtcGetMode(), action.value);
        break;

    case ACTION_BR_WORK:
        screenSet(SCREEN_BRIGHTNESS);
        swTimSetDisplay(5000);
        screenChangeBrighness(action.type, action.value);
        break;

    case ACTION_TIMER_EXPIRED:
        screenSet(RTC_NOEDIT);
        if (SCREEN_STANDBY != screen) {
            screenSet(screenGetDefault());
        }
        break;
    default:
        break;
    }
}
