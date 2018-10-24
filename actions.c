#include "actions.h"

#include "audio/audio.h"
#include "eemul.h"
#include "input.h"
#include "menu.h"
#include "rtc.h"
#include "screen.h"
#include "spectrum.h"
#include "timers.h"
#include "tuner/tuner.h"

static Action action = {ACTION_NONE, 0};

static ScreenParam scrPar;

static void actionSet(ActionType type, int16_t value)
{
    action.type = type;
    action.value = value;
}

static void actionNextAudioParam(AudioProc *aProc)
{
    do {
        scrPar.audio++;
        if (scrPar.audio >= AUDIO_PARAM_END)
            scrPar.audio = AUDIO_PARAM_VOLUME;
    } while (aProc->item[scrPar.audio].grid->step == 0);
}

static void actionNextAudioInput(AudioProc *aProc)
{
    scrPar.input++;
    if (scrPar.input >= aProc->inCnt)
        scrPar.input = 0;
}

static void actionChangeCurrentInput(int8_t diff)
{
    screenSet(SCREEN_AUDIO_PARAM);
    scrPar.audio = AUDIO_PARAM_GAIN;
    actionSet(ACTION_AUDIO_PARAM_CHANGE, diff);
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
            action.type = ACTION_AUDIO_INPUT;
            break;
        case BTN_D2:
            action.type = ACTION_RTC_MODE;
            break;
        case BTN_D3:
            action.type = ACTION_PREV;
            break;
        case BTN_D4:
            action.type = ACTION_NEXT;
            break;
        case BTN_D5:
            action.type = ACTION_OK;
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
            if (screen == SCREEN_STANDBY)
                action.type = ACTION_TEST;
            break;
        case BTN_D2:
            break;
        case BTN_D3:
            break;
        case BTN_D4:
            break;
        case BTN_D5:
            if (screen == SCREEN_STANDBY) {
                action.type = ACTION_MENU_SELECT;
                action.value = MENU_SETUP_AUDIO;
            }
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
    case ACTION_PREV:
        switch (screen) {
        case SCREEN_TUNER:
            action.type = ACTION_TUNER_PREV;
            break;
        case SCREEN_MENU:
            action.type = ACTION_MENU_CHANGE;
            action.value = -1;
            break;
        default:
            break;
        }
        break;
    case ACTION_NEXT:
        switch (screen) {
        case SCREEN_TUNER:
            action.type = ACTION_TUNER_NEXT;
            break;
        case SCREEN_MENU:
            action.type = ACTION_MENU_CHANGE;
            action.value = +1;
            break;
        default:
            break;
        }
        break;
    case ACTION_OK:
        switch (screen) {
        case SCREEN_MENU:
            action.type = ACTION_MENU_SELECT;
            action.value = menuGetFirstChild();
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    if (SCREEN_STANDBY == screen &&
        (ACTION_STANDBY != action.type &&
         ACTION_TEST != action.type &&
         ACTION_MENU_SELECT != action.type)) {
        actionSet(ACTION_NONE, 0);
    }

    if (SCREEN_MENU == screen &&
        (ACTION_MENU_CHANGE != action.type &&
         ACTION_MENU_SELECT != action.type)) {
        actionSet(ACTION_NONE, 0);
    }
}

static void actionHandleTimers(void)
{
    if (ACTION_NONE == action.type) {
        if (swTimGetDisplay() == 0) {
            action.type = ACTION_TIMER_EXPIRED;
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
            case SCREEN_STANDBY:
                break;
            case SCREEN_TIME:
                actionSet(ACTION_RTC_CHANGE, encCnt);
                break;
            case SCREEN_BRIGHTNESS:
                actionSet(ACTION_BR_WORK, encCnt);
                break;
            case SCREEN_AUDIO_INPUT:
                actionChangeCurrentInput(encCnt);
                break;
            case SCREEN_MENU:
                actionSet(ACTION_MENU_CHANGE, encCnt);
                break;
            case SCREEN_SPECTRUM:
                screenSet(SCREEN_AUDIO_PARAM);
                scrPar.audio = AUDIO_PARAM_VOLUME;
            default:
                actionSet(ACTION_AUDIO_PARAM_CHANGE, encCnt);
                break;
            }
        }
    }
}

Action actionUserGet(void)
{
    actionSet(ACTION_NONE, 0);

    actionHandleButtons();
    actionRemapButtons();
    actionRemapActions();

    actionHandleEncoder();

    actionHandleTimers();

    return action;
}

void actionHandle(Action action, uint8_t visible)
{
    Screen screen = screenGet();
    AudioProc *aProc = audioProcGet();
    int16_t dispTime = 0;

    switch (action.type) {
    case ACTION_STANDBY:
        if (action.value == CMD_OFF) {
            screen = SCREEN_TIME;
            dispTime = 1000;
            tunerSetPower(1);
        } else {
            dispTime = SW_TIM_OFF;
            screen = SCREEN_STANDBY;
            rtcSetMode(RTC_NOEDIT);
            screenSaveSettings();
            audioPowerOff();
            tunerSetPower(0);
        }
        break;

    case ACTION_RTC_MODE:
        if (screen == SCREEN_TIME) {
            dispTime = 15000;
            rtcModeNext();
        } else {
            dispTime = 5000;
            rtcSetMode(RTC_NOEDIT);
            screen = SCREEN_TIME;
        }
        break;
    case ACTION_RTC_CHANGE:
        dispTime = 5000;
        rtcChangeTime(rtcGetMode(), action.value);
        break;
    case ACTION_RTC_SET_HOUR:
    case ACTION_RTC_SET_MIN:
    case ACTION_RTC_SET_SEC:
    case ACTION_RTC_SET_DATE:
    case ACTION_RTC_SET_MONTH:
    case ACTION_RTC_SET_YEAR:
        dispTime = 5000;
        rtcSetTime(action.type - ACTION_RTC_SET_HOUR, action.value);
        break;

    case ACTION_AUDIO_INPUT:
        dispTime = 5000;
        if (screen == SCREEN_AUDIO_INPUT) {
            actionNextAudioInput(aProc);
            audioSetInput(scrPar.input);
        } else {
            screen = SCREEN_AUDIO_INPUT;
            scrPar.input = aProc->input;
        }
        break;
    case ACTION_OK:
        dispTime = 5000;
        if (screen == SCREEN_AUDIO_PARAM) {
            actionNextAudioParam(aProc);
        } else {
            screen = SCREEN_AUDIO_PARAM;
            scrPar.audio = AUDIO_PARAM_VOLUME;
        }
        break;
    case ACTION_AUDIO_PARAM_CHANGE:
        dispTime = 5000;
        audioChangeParam(scrPar.audio, action.value);
        break;
    case ACTION_TUNER_PREV:
        tunerNextStation(TUNER_DIR_DOWN);
        break;
    case ACTION_TUNER_NEXT:
        tunerNextStation(TUNER_DIR_UP);
        break;

    case ACTION_BR_WORK:
        screen = SCREEN_BRIGHTNESS;
        dispTime = 5000;
        screenChangeBrighness(action.type, action.value);
        break;

    case ACTION_TIMER_EXPIRED:
        rtcSetMode(RTC_NOEDIT);
        if (SCREEN_STANDBY != screen) {
            switch (screen) {
            case SCREEN_MENU:
                screen = SCREEN_STANDBY;
                break;
            default:
                screen = screenGetDefault();
                break;
            }
        }
        dispTime = SW_TIM_OFF;
        break;

    case ACTION_MENU_SELECT:
        menuSetActive(action.value);
        scrPar.parent = menuGet()->parent;
        screen = SCREEN_MENU;
        dispTime = 10000;
        break;
    case ACTION_MENU_CHANGE:
        menuChange(action.value);
        scrPar.parent = menuGet()->parent;
        dispTime = 10000;
        break;

    default:
        break;
    }

    if (visible) {
        screenSet(screen);
        screenSetParam(scrPar);
        if (dispTime) {
            swTimSetDisplay(dispTime);
        }
    }
}
