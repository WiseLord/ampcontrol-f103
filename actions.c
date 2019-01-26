#include "actions.h"

#include <stddef.h>
#include <string.h>

#include "audio/audio.h"
#include "eemap.h"
#include "input.h"
#include "menu.h"
#include "pins.h"
#include "rtc.h"
#include "screen.h"
#include "spectrum.h"
#include "swtimers.h"
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
    } while (aProc->par.item[scrPar.tune].grid == NULL && scrPar.tune != AUDIO_TUNE_VOLUME);
}

static void actionNextAudioInput(AudioProc *aProc)
{
    scrPar.input++;
    if (scrPar.input >= aProc->par.inCnt)
        scrPar.input = 0;
}

static void actionGetButtons(void)
{
    CmdBtn cmdBtn = getBtnCmd();

    if (cmdBtn & 0xFF00) {
        action.type = ACTION_BTN_LONG;
        action.value = cmdBtn >> 8;
    } else if (cmdBtn & 0x00FF) {
        action.type = ACTION_BTN_SHORT;
        action.value = cmdBtn & 0xFF;
    }

}

static void actionGetEncoder(void)
{
    int8_t encVal = getEncoder();

    if (encVal) {
        action.type = ACTION_ENCODER;
        action.value = encVal;
    }
}

static void actionGetRemote(void)
{
    RcData rcData = rcRead(true);

    if (rcData.ready) {
        RcCmd cmd = rcGetCmd(&rcData);
        if (rcData.repeat) {
            // Allow repeat only following commands
            if (cmd == RC_CMD_VOL_UP ||
                cmd == RC_CMD_VOL_DOWN) {
                if (swTimGetRcRepeat() > 0)
                    return;
            } else {
                return;
            }
        } else {
            swTimSetRcRepeat(400);  // Allow repeat after this time
        }

        if (screenGet() == SCREEN_MENU) {
            action.type = ACTION_MENU_CHANGE;
            action.value = 0;
        } else {
            action.type = ACTION_REMOTE;
            action.value = (int16_t)cmd;
        }
    }
}

static void actionGetTimers(void)
{
    if (swTimGetDisplay() == 0) {
        action.type = ACTION_DISP_EXPIRED;
    } else if (swTimGetInitHw() == 0) {
        action.type = ACTION_INIT_HW;
    }
}


static void actionRemapBtnShort(void)
{
    Screen screen = screenGet();

    switch (action.value) {
    case BTN_D0:
        action.type = ACTION_STANDBY;
        action.value = FLAG_SWITCH;
        break;
    case BTN_D1:
        action.type = ACTION_AUDIO_INPUT;
        break;
    case BTN_D2:
        if (screen == SCREEN_MENU) {
            action.type = ACTION_MENU_BACK;
        } else {
            action.type = ACTION_RTC_MODE;
        }
        break;
    case BTN_D3:
        action.type = ACTION_PREV;
        break;
    case BTN_D4:
        action.type = ACTION_NEXT;
        break;
    case BTN_D5:
        action.type = ACTION_MENU;
        break;
    default:
        break;
    }
}

static void actionRemapBtnLong(void)
{
    Screen screen = screenGet();

    switch (action.value) {
    case BTN_D0:
        action.type = ACTION_BR_WORK;
        action.value = 0;
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
            action.value = MENU_SETUP_LANG;
        }
        break;
    default:
        break;
    }
}

static void actionRemapRemote(void)
{
    Screen screen = screenGet();

    if (SCREEN_STANDBY == screen &&
        action.value != RC_CMD_STBY_SWITCH &&
        action.value != RC_CMD_STBY_EXIT)
        return;

    switch (action.value) {
    case RC_CMD_STBY_SWITCH:
        action.type = ACTION_STANDBY;
        action.value = FLAG_SWITCH;
        break;
    case RC_CMD_MUTE:
        action.type = ACTION_AUDIO_MUTE;
        action.value = FLAG_SWITCH;
        break;
    case RC_CMD_VOL_UP:
        action.type = ACTION_ENCODER;
        action.value = +1;
        break;
    case RC_CMD_VOL_DOWN:
        action.type = ACTION_ENCODER;
        action.value = -1;
        break;
    case RC_CMD_MENU:
        action.type = ACTION_MENU;
        break;
    case RC_CMD_CHAN_NEXT:
        action.type = ACTION_NEXT;
        break;
    case RC_CMD_CHAN_PREV:
        action.type = ACTION_PREV;
        break;
    case RC_CMD_DIG_0:
    case RC_CMD_DIG_1:
    case RC_CMD_DIG_2:
    case RC_CMD_DIG_3:
    case RC_CMD_DIG_4:
    case RC_CMD_DIG_5:
    case RC_CMD_DIG_6:
    case RC_CMD_DIG_7:
    case RC_CMD_DIG_8:
    case RC_CMD_DIG_9:
        action.type = ACTION_DIGIT;
        action.value -= RC_CMD_DIG_0;
        break;
    case RC_CMD_IN_NEXT:
        action.type = ACTION_AUDIO_INPUT;
        break;
    case RC_CMD_IN_PREV:
        break;
    case RC_CMD_IN_0:
    case RC_CMD_IN_1:
    case RC_CMD_IN_2:
    case RC_CMD_IN_3:
    case RC_CMD_IN_4:
    case RC_CMD_IN_5:
    case RC_CMD_IN_6:
    case RC_CMD_IN_7:
        break;

    case RC_CMD_BASS_UP:
        screenSet(SCREEN_AUDIO_PARAM);
        scrPar.tune = AUDIO_TUNE_BASS;
        action.type = ACTION_ENCODER;
        action.value = +1;
        break;
    case RC_CMD_BASS_DOWN:
        screenSet(SCREEN_AUDIO_PARAM);
        scrPar.tune = AUDIO_TUNE_BASS;
        action.type = ACTION_ENCODER;
        action.value = -1;
        break;
    case RC_CMD_MIDDLE_UP:
        screenSet(SCREEN_AUDIO_PARAM);
        scrPar.tune = AUDIO_TUNE_MIDDLE;
        action.type = ACTION_ENCODER;
        action.value = +1;
        break;
    case RC_CMD_MIDDLE_DOWN:
        screenSet(SCREEN_AUDIO_PARAM);
        scrPar.tune = AUDIO_TUNE_MIDDLE;
        action.type = ACTION_ENCODER;
        action.value = -1;
        break;
    case RC_CMD_TREBLE_UP:
        screenSet(SCREEN_AUDIO_PARAM);
        scrPar.tune = AUDIO_TUNE_TREBLE;
        action.type = ACTION_ENCODER;
        action.value = +1;
        break;
    case RC_CMD_TREBLE_DOWN:
        screenSet(SCREEN_AUDIO_PARAM);
        scrPar.tune = AUDIO_TUNE_TREBLE;
        action.type = ACTION_ENCODER;
        action.value = -1;
        break;

    case RC_CMD_LOUDNESS:
        action.type = ACTION_AUDIO_LOUDNESS;
        action.value = FLAG_SWITCH;
        break;
    case RC_CMD_SURROUND:
        action.type = ACTION_AUDIO_SURROUND;
        action.value = FLAG_SWITCH;
        break;
    case RC_CMD_EFFECT_3D:
        action.type = ACTION_AUDIO_EFFECT3D;
        action.value = FLAG_SWITCH;
        break;
    case RC_CMD_TONE_BYPASS:
        action.type = ACTION_AUDIO_BYPASS;
        action.value = FLAG_SWITCH;
        break;

    case RC_CMD_TIME:
        action.type = ACTION_RTC_MODE;
        break;
    case RC_CMD_STBY_ENTER:
        action.type = ACTION_STANDBY;
        action.value = FLAG_ON;
        break;
    case RC_CMD_STBY_EXIT:
        action.type = ACTION_STANDBY;
        action.value = FLAG_OFF;
        break;
    default:
        break;
    }
}

static void actionRemapEncoder()
{
    Screen screen = screenGet();

    if (SCREEN_STANDBY == screen)
        return;

    int16_t encCnt = action.value;

    switch (screen) {
    case SCREEN_TIME:
        if (rtcGetMode() == RTC_NOEDIT) {
            actionSet(ACTION_AUDIO_PARAM_CHANGE, encCnt);
        } else {
            actionSet(ACTION_RTC_CHANGE, encCnt);
        }
        break;
    case SCREEN_BRIGHTNESS:
        actionSet(ACTION_BR_WORK, encCnt);
        break;
    case SCREEN_MENU:
        actionSet(ACTION_MENU_CHANGE, encCnt);
        break;
    default:
        actionSet(ACTION_AUDIO_PARAM_CHANGE, encCnt);
        break;
    }

    if (ACTION_AUDIO_PARAM_CHANGE == action.type) {
        screenSet(SCREEN_AUDIO_PARAM);
        switch (screen) {
        case SCREEN_AUDIO_INPUT:
            scrPar.tune = AUDIO_TUNE_GAIN;
            break;
        case SCREEN_SPECTRUM:
            scrPar.tune = AUDIO_TUNE_VOLUME;
        default:
            break;
        }
    }
}


static void actionRemapCommon(void)
{
    Screen screen = screenGet();
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];
    Menu *menu = menuGet();

    switch (action.type) {
    case ACTION_STANDBY:
        if (FLAG_SWITCH == action.value) {
            action.value = (SCREEN_STANDBY == screen ? FLAG_OFF : FLAG_ON);
        }
        break;
    case ACTION_PREV:
        switch (screen) {
        case SCREEN_MENU:
            action.type = ACTION_MENU_CHANGE;
            action.value = -1;
            break;
        default:
            if (inType == IN_TUNER) {
                action.type = ACTION_TUNER_PREV;
            }
            break;
        }
        break;
    case ACTION_NEXT:
        switch (screen) {
        case SCREEN_MENU:
            action.type = ACTION_MENU_CHANGE;
            action.value = +1;
            break;
        default:
            if (inType == IN_TUNER) {
                action.type = ACTION_TUNER_NEXT;
            }
            break;
        }
        break;
    case ACTION_MENU:
        switch (screen) {
        case SCREEN_MENU:
            action.type = ACTION_MENU_SELECT;
            action.value = (int16_t)(menuGetFirstChild());
            break;
        default:
            break;
        }
        break;
    case ACTION_MENU_BACK:
        if (menu->selected) {
            menu->selected = false;
        } else if (menuIsTop()) {
            // TODO: Return to original screen called menu
            action.type = ACTION_STANDBY;
            action.value = FLAG_ON;
        } else {
            action.type = ACTION_MENU_SELECT;
            action.value = (int16_t)menu->parent;
        }
        break;
    case ACTION_AUDIO_MUTE:
        if (FLAG_SWITCH == action.value) {
            action.value = !aProc->par.mute;
        }
        break;
    default:
        break;
    }

    if (SCREEN_STANDBY == screen &&
        (ACTION_STANDBY != action.type &&
         ACTION_REMOTE != action.type &&
         ACTION_MENU_SELECT != action.type)) {
        actionSet(ACTION_NONE, 0);
    }

    if (SCREEN_MENU == screen &&
        (ACTION_STANDBY != action.type &&
         ACTION_MENU_BACK != action.type &&
         ACTION_MENU_CHANGE != action.type &&
         ACTION_MENU_SELECT != action.type)) {
        actionSet(ACTION_NONE, 0);
    }
}

Action actionUserGet(void)
{
    actionSet(ACTION_NONE, 0);

    actionGetButtons();

    if (ACTION_NONE == action.type) {
        actionGetEncoder();
    }

    if (ACTION_NONE == action.type) {
        actionGetRemote();
    }

    if (ACTION_NONE == action.type) {
        actionGetTimers();
    }

    switch (action.type) {
    case ACTION_BTN_SHORT:
        actionRemapBtnShort();
        break;
    case ACTION_BTN_LONG:
        actionRemapBtnLong();
        break;
    case ACTION_REMOTE:
        actionRemapRemote();
        break;
    default:
        break;
    }

    if (ACTION_ENCODER == action.type) {
        actionRemapEncoder();
    }

    actionRemapCommon();

    return action;
}


void actionHandle(Action action, uint8_t visible)
{
    Screen screen = screenGet();
    AudioProc *aProc = audioGet();
    Tuner *tuner = tunerGet();
    int16_t dispTime = 0;

    switch (action.type) {
    case ACTION_STANDBY:
        if (action.value == FLAG_OFF) {
            screen = SCREEN_TIME;
            dispTime = 800;

            audioReadSettings();
            tunerReadSettings();

            pinsSetStby(true);      // ON via relay
            swTimSetInitHw(500);
        } else {
            dispTime = SW_TIM_OFF;
            screen = SCREEN_STANDBY;
            memset(&scrPar, 0, sizeof (scrPar));
            rtcSetMode(RTC_NOEDIT);
            screenSaveSettings();

            audioSetMute(true);
            audioSetPower(false);

            tunerSetMute(true);
            tunerSetPower(false);

            pinsSetStby(false);     // OFF via relay
        }
        break;
    case ACTION_INIT_HW:
        swTimSetInitHw(SW_TIM_OFF);

        tunerInit();
        tunerSetPower(true);
        tunerSetMute(false);
        tunerSetFreq(tuner->par.freq);

        audioInit();
        audioSetPower(true);
        break;
    case ACTION_DISP_EXPIRED:
        memset(&scrPar, 0, sizeof (scrPar));
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
    case ACTION_DIGIT:
        if (screen == SCREEN_TIME) {
            dispTime = 5000;
            rtcEditTime(rtcGetMode(), (int8_t)(action.value));
        }
        break;

    case ACTION_MENU:
        dispTime = 5000;
        if (screen == SCREEN_AUDIO_PARAM) {
            actionNextAudioParam(aProc);
        } else {
            screen = SCREEN_AUDIO_PARAM;
            scrPar.tune = AUDIO_TUNE_VOLUME;
        }
        break;
    case ACTION_RTC_MODE:
        if (screen == SCREEN_TIME) {
            dispTime = 15000;
            rtcChangeMode(+1);
        } else {
            dispTime = 5000;
            rtcSetMode(RTC_NOEDIT);
            screen = SCREEN_TIME;
        }
        break;
    case ACTION_RTC_CHANGE:
        dispTime = 5000;
        rtcChangeTime(rtcGetMode(), (int8_t)(action.value));
        break;
    case ACTION_RTC_SET_HOUR:
    case ACTION_RTC_SET_MIN:
    case ACTION_RTC_SET_SEC:
    case ACTION_RTC_SET_DATE:
    case ACTION_RTC_SET_MONTH:
    case ACTION_RTC_SET_YEAR:
        dispTime = 5000;
        rtcSetTime((int8_t)(action.type - ACTION_RTC_SET_HOUR), (int8_t)(action.value));
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
    case ACTION_AUDIO_PARAM_CHANGE:
        dispTime = 5000;
        audioChangeTune(scrPar.tune, (int8_t)(action.value));
        break;

    case ACTION_AUDIO_MUTE:
        audioSetMute(action.value);
        break;

    case ACTION_TUNER_PREV:
        screen = SCREEN_TUNER;
        tunerNextStation(TUNER_DIR_DOWN);
        break;
    case ACTION_TUNER_NEXT:
        screen = SCREEN_TUNER;
        tunerNextStation(TUNER_DIR_UP);
        break;
    case ACTION_BR_STBY:
    case ACTION_BR_WORK:
        screen = SCREEN_BRIGHTNESS;
        dispTime = 5000;
        screenChangeBrighness((BrMode)(action.type - ACTION_BR_STBY), (int8_t)(action.value));
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
        screen = SCREEN_MENU;
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
