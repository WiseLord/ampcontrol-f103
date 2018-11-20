#include "actions.h"

#include <stddef.h>

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
        scrPar.tune++;
        if (scrPar.tune >= AUDIO_TUNE_END)
            scrPar.tune = AUDIO_TUNE_VOLUME;
    } while (aProc->par.item[scrPar.tune].grid == NULL);
}

static void actionNextAudioInput(AudioProc *aProc)
{
    scrPar.input++;
    if (scrPar.input >= aProc->par.inCnt)
        scrPar.input = 0;
}

static void actionChangeCurrentInput(int8_t diff)
{
    screenSet(SCREEN_AUDIO_PARAM);
    scrPar.tune = AUDIO_TUNE_GAIN;
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
            action.value = STBY_SWITCH;
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
        if (STBY_SWITCH == action.value) {
            action.value = (SCREEN_STANDBY == screen ? STBY_EXIT : STBY_ENTER);
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
         ACTION_RC_CMD != action.type &&
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
            action.type = ACTION_DISP_EXPIRED;
        } else if (swTimGetInitHw() == 0) {
            action.type = ACTION_INIT_HW;
        }
    }
}

static void actionHandleEncoder(int8_t diff)
{
    Screen screen = screenGet();

    if (ACTION_NONE == action.type) {
        int8_t encCnt = diff ? diff : getEncoder();

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
                scrPar.tune = AUDIO_TUNE_VOLUME;
            default:
                actionSet(ACTION_AUDIO_PARAM_CHANGE, encCnt);
                break;
            }
        }
    }
}

static void actionHandleRemote(void)
{
    Screen screen = screenGet();

    RcData rcData = rcRead(true);

    if (!rcData.ready)
        return;

    if (screen == SCREEN_MENU) {
        if (!rcData.repeat) {
            action.type = ACTION_MENU_CHANGE;
            action.value = 0;
        }
    } else {
        RcCmd rcCmd = rcGetCmd(&rcData);

        // Emulate encoder
        switch (rcCmd) {
        case RC_CMD_VOL_UP:
            actionHandleEncoder(+1);
            break;
        case RC_CMD_VOL_DOWN:
            actionHandleEncoder(-1);
            break;
        default:
            break;
        }

        if (!rcData.repeat) {
            switch (rcCmd) {
            case RC_CMD_STBY_SWITCH:
                actionSet(ACTION_STANDBY, STBY_SWITCH);
                break;
            default:
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
    actionHandleRemote();
    actionRemapActions();

    actionHandleEncoder(0);

    actionHandleTimers();

    return action;
}

void actionHandle(Action action, uint8_t visible)
{
    Screen screen = screenGet();
    AudioProc *aProc = audioGet();
    int16_t dispTime = 0;

    switch (action.type) {
    case ACTION_STANDBY:
        if (action.value == STBY_EXIT) {
            screen = SCREEN_TIME;
            dispTime = 800;
            swTimSetInitHw(500);
        } else {
            dispTime = SW_TIM_OFF;
            screen = SCREEN_STANDBY;
            rtcSetMode(RTC_NOEDIT);
            screenSaveSettings();

            audioSetFlag(AUDIO_FLAG_MUTE, true);
            audioSetPower(false);

            tunerSetFlag(TUNER_FLAG_MUTE, true);
            tunerSetPower(false);
        }
        break;
    case ACTION_INIT_HW:
        swTimSetInitHw(SW_TIM_OFF);
        tunerInit();
        tunerSetPower(true);
        tunerSetFreq(tunerGet()->freq);
        tunerSetFlag(TUNER_FLAG_MUTE, false);

        audioInit();
        audioSetPower(true);
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
            scrPar.input = aProc->par.input;
        }
        break;
    case ACTION_OK:
        dispTime = 5000;
        if (screen == SCREEN_AUDIO_PARAM) {
            actionNextAudioParam(aProc);
        } else {
            screen = SCREEN_AUDIO_PARAM;
            scrPar.tune = AUDIO_TUNE_VOLUME;
        }
        break;
    case ACTION_AUDIO_PARAM_CHANGE:
        dispTime = 5000;
        audioChangeTune(scrPar.tune, action.value);
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

    case ACTION_DISP_EXPIRED:
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
