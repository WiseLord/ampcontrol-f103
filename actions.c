#include "actions.h"

#include <stddef.h>
#include <string.h>

#include "audio/audio.h"
#include "canvas/canvas.h"
#include "canvas/layout.h"
#include "eemap.h"
#include "input.h"
#include "menu.h"
#include "pins.h"
#include "rtc.h"
#include "screen.h"
#include "spectrum.h"
#include "swtimers.h"
#include "tuner/stations.h"
#include "tuner/tuner.h"

static void actionGetButtons(void);
static void actionGetEncoder(void);
static void actionGetRemote(void);
static void actionGetTimers(void);
static void actionRemapBtnShort(void);
static void actionRemapBtnLong(void);
static void actionRemapRemote(void);
static void actionRemapCommon(void);
static void actionRemapNavigate(void);
static void actionRemapEncoder(void);

static Action action = {ACTION_STANDBY, false, FLAG_ON, SCREEN_STANDBY, {0}, 0, ACTION_STANDBY};

static void actionSet(ActionType type, int16_t value)
{
    action.type = type;
    action.value = value;
}

static void actionSetScreen(Screen screen, int16_t timeout)
{
    action.screen = screen;
    action.timeout = timeout;
}

static void actionDispExpired(Screen screen)
{
    memset(&action.param, 0, sizeof (action.param));
    rtcSetMode(RTC_NOEDIT);

    switch (screen) {
    case SCREEN_STANDBY:
    case SCREEN_MENU:
        actionSetScreen(SCREEN_STANDBY, SW_TIM_OFF); // TODO: Return to parent screen caused menu
        break;
    default:
        actionSetScreen(screenGetDefault(), SW_TIM_OFF);
        break;
    }
}

static void actionNavigateMenu(RcCmd cmd)
{
    Menu *menu = menuGet();

    switch (cmd) {
    case RC_CMD_NAV_OK:
    case RC_CMD_NAV_RIGHT:
        actionSet(ACTION_MENU_SELECT, (int16_t)(menuGetFirstChild()));
        break;
    case RC_CMD_NAV_BACK:
    case RC_CMD_NAV_LEFT:
        if (menu->selected) {
            menu->selected = false;
        } else if (menuIsTop()) {
            // TODO: Return to original screen called menu
            actionSet(ACTION_STANDBY, FLAG_ON);
        } else {
            actionSet(ACTION_MENU_SELECT, (int16_t)menu->parent);
        }
        break;
    case RC_CMD_NAV_UP:
        actionSet(ACTION_MENU_CHANGE, menu->selected ? +1 : -1);
        break;
    case RC_CMD_NAV_DOWN:
        actionSet(ACTION_MENU_CHANGE, menu->selected ? -1 : +1);
        break;
    }
}

static void actionNavigateCommon(RcCmd cmd)
{
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];
    Screen screen = screenGet();

    switch (cmd) {
    case RC_CMD_NAV_OK:
        if (screen == SCREEN_TEXTEDIT) {
            action.type = ACTION_TEXTEDIT_APPLY;
        }
        break;
    case RC_CMD_NAV_BACK:
        if (screen == SCREEN_TEXTEDIT) {
            action.type = ACTION_TEXTEDIT_CANCEL;
        } else {
            actionSet(ACTION_DISP_EXPIRED, 0);
        }
        break;
    case RC_CMD_NAV_RIGHT:
        if (screen == SCREEN_TEXTEDIT) {
            action.type = ACTION_TEXTEDIT_ADD_CHAR;
        } else if (inType == IN_TUNER) {
            action.type = ACTION_TUNER_NEXT;
        }
        break;
    case RC_CMD_NAV_LEFT:
        if (screen == SCREEN_TEXTEDIT) {
            action.type = ACTION_TEXTEDIT_DEL_CHAR;
        } else if (inType == IN_TUNER) {
            action.type = ACTION_TUNER_PREV;
        }
        break;
    case RC_CMD_NAV_UP:
        if (screen == SCREEN_TEXTEDIT) {
            actionSet(ACTION_ENCODER, -1);
        } else {
            actionSet(ACTION_ENCODER, +1);
        }
        break;
    case RC_CMD_NAV_DOWN:
        if (screen == SCREEN_TEXTEDIT) {
            actionSet(ACTION_ENCODER, +1);
        } else {
            actionSet(ACTION_ENCODER, -1);
        }
        break;
    }
}

static void actionNextAudioParam(AudioProc *aProc)
{
    do {
        action.param.tune++;
        if (action.param.tune >= AUDIO_TUNE_END)
            action.param.tune = AUDIO_TUNE_VOLUME;
    } while (aProc->par.item[action.param.tune].grid == NULL && action.param.tune != AUDIO_TUNE_VOLUME);
}

static void actionNextAudioInput(AudioProc *aProc)
{
    action.param.input++;
    if (action.param.input >= aProc->par.inCnt)
        action.param.input = 0;
}

static void actionGetButtons(void)
{
    CmdBtn cmdBtn = getBtnCmd();

    if (cmdBtn & 0xFF00) {
        actionSet(ACTION_BTN_LONG, cmdBtn >> 8);
    } else if (cmdBtn & 0x00FF) {
        actionSet(ACTION_BTN_SHORT, cmdBtn & 0xFF);
    }
}

static void actionGetEncoder(void)
{
    int8_t encVal = getEncoder();

    if (encVal) {
        actionSet(ACTION_ENCODER, encVal);
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

        actionSet(ACTION_REMOTE, (int16_t)cmd);
    }
}

static void actionGetTimers(void)
{
    if (swTimGetDisplay() == 0) {
        actionSet(ACTION_DISP_EXPIRED, 0);
    } else if (swTimGetInitHw() == 0) {
        actionSet(ACTION_INIT_HW, 0);
    }
}

static void actionRemapBtnShort(void)
{
    switch (action.value) {
    case BTN_D0:
        actionSet(ACTION_STANDBY, FLAG_SWITCH);
        break;
    case BTN_D1:
        actionSet(ACTION_AUDIO_INPUT, -1);
        break;
    case BTN_D2:
        actionSet(ACTION_NAVIGATE, RC_CMD_NAV_BACK);
        break;
    case BTN_D3:
        action.type = ACTION_PREV;
        break;
    case BTN_D4:
        action.type = ACTION_NEXT;
        break;
    case BTN_D5:
        action.type = ACTION_AUDIO_MENU;
        break;
    case ENC_A:
        actionSet(ACTION_ENCODER, -1);
        break;
    case ENC_B:
        actionSet(ACTION_ENCODER, +1);
        break;
    default:
        break;
    }
}

static void actionRemapBtnLong(void)
{
    Screen screen = screenGet();
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    switch (action.value) {
    case BTN_D0:
        actionSet(ACTION_BR_WORK, 0);
        break;
    case BTN_D1:
        action.type = ACTION_RTC_MODE;
        break;
    case BTN_D2:
        if (inType == IN_TUNER) {
            action.type = ACTION_TUNER_EDIT_NAME;
        }
        break;
    case BTN_D3:
        action.type = ACTION_PREV;
        break;
    case BTN_D4:
        action.type = ACTION_NEXT;
        break;
    case BTN_D5:
        if (screen == SCREEN_STANDBY) {
            actionSet(ACTION_MENU_SELECT, MENU_SETUP_LANG);
        }
        break;
    case ENC_A:
        actionSet(ACTION_ENCODER, -1);
        break;
    case ENC_B:
        actionSet(ACTION_ENCODER, +1);
        break;
    default:
        break;
    }
}

static void actionRemapRemote(void)
{
    Screen screen = screenGet();

    if (screen == SCREEN_MENU) {
        Menu *menu = menuGet();
        if ((menu->parent == MENU_SETUP_RC) && (menu->selected)) {
            actionSet(ACTION_MENU_CHANGE, 0);
            return;
        }
    }

    if (SCREEN_STANDBY == screen &&
            action.value == RC_CMD_MENU) {
        actionSet(ACTION_MENU_SELECT, MENU_SETUP_LANG);
        return;
    }

    if (SCREEN_STANDBY == screen &&
            action.value != RC_CMD_STBY_SWITCH &&
            action.value != RC_CMD_STBY_EXIT)
        return;

    switch (action.value) {
    case RC_CMD_STBY_SWITCH:
        actionSet(ACTION_STANDBY, FLAG_SWITCH);
        break;

    case RC_CMD_MUTE:
        actionSet(ACTION_AUDIO_MUTE, FLAG_SWITCH);
        break;
    case RC_CMD_VOL_UP:
        actionSet(ACTION_ENCODER, +1);
        break;
    case RC_CMD_VOL_DOWN:
        actionSet(ACTION_ENCODER, -1);
        break;

    case RC_CMD_MENU:
        action.type = ACTION_AUDIO_MENU;
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
        actionSet(ACTION_DIGIT, action.value - RC_CMD_DIG_0);
        break;

    case RC_CMD_IN_NEXT:
        actionSet(ACTION_AUDIO_INPUT, -1);
        break;

    case RC_CMD_NAV_OK:
    case RC_CMD_NAV_BACK:
    case RC_CMD_NAV_RIGHT:
    case RC_CMD_NAV_UP:
    case RC_CMD_NAV_LEFT:
    case RC_CMD_NAV_DOWN:
        actionSet(ACTION_NAVIGATE, action.value);
        break;

    case RC_CMD_IN_0:
    case RC_CMD_IN_1:
    case RC_CMD_IN_2:
    case RC_CMD_IN_3:
    case RC_CMD_IN_4:
    case RC_CMD_IN_5:
    case RC_CMD_IN_6:
    case RC_CMD_IN_7:
        actionSet(ACTION_AUDIO_INPUT, action.value - RC_CMD_IN_0);
        break;

    case RC_CMD_BASS_UP:
        screenSet(SCREEN_AUDIO_PARAM);
        action.param.tune = AUDIO_TUNE_BASS;
        actionSet(ACTION_ENCODER, +1);
        break;
    case RC_CMD_BASS_DOWN:
        screenSet(SCREEN_AUDIO_PARAM);
        action.param.tune = AUDIO_TUNE_BASS;
        actionSet(ACTION_ENCODER, -1);
        break;
    case RC_CMD_MIDDLE_UP:
        screenSet(SCREEN_AUDIO_PARAM);
        action.param.tune = AUDIO_TUNE_MIDDLE;
        actionSet(ACTION_ENCODER, +1);
        break;
    case RC_CMD_MIDDLE_DOWN:
        screenSet(SCREEN_AUDIO_PARAM);
        action.param.tune = AUDIO_TUNE_MIDDLE;
        actionSet(ACTION_ENCODER, -1);
        break;
    case RC_CMD_TREBLE_UP:
        screenSet(SCREEN_AUDIO_PARAM);
        action.param.tune = AUDIO_TUNE_TREBLE;
        actionSet(ACTION_ENCODER, +1);
        break;
    case RC_CMD_TREBLE_DOWN:
        screenSet(SCREEN_AUDIO_PARAM);
        action.param.tune = AUDIO_TUNE_TREBLE;
        actionSet(ACTION_ENCODER, -1);
        break;

    case RC_CMD_LOUDNESS:
        actionSet(ACTION_AUDIO_LOUDNESS, FLAG_SWITCH);
        break;
    case RC_CMD_SURROUND:
        actionSet(ACTION_AUDIO_SURROUND, FLAG_SWITCH);
        break;
    case RC_CMD_EFFECT_3D:
        actionSet(ACTION_AUDIO_EFFECT3D, FLAG_SWITCH);
        break;
    case RC_CMD_TONE_BYPASS:
        actionSet(ACTION_AUDIO_BYPASS, FLAG_SWITCH);
        break;

    case RC_CMD_TIME:
        action.type = ACTION_RTC_MODE;
        break;
    case RC_CMD_STBY_ENTER:
        actionSet(ACTION_STANDBY, FLAG_ON);
        break;
    case RC_CMD_STBY_EXIT:
        actionSet(ACTION_STANDBY, FLAG_OFF);
        break;
    default:
        break;
    }
}

static void actionRemapNavigate(void)
{
    Screen screen = screenGet();

    switch (screen) {
    case SCREEN_MENU:
        actionNavigateMenu((RcCmd)action.value);
        break;
    default:
        actionNavigateCommon((RcCmd)action.value);
        break;
    }
}

static void actionRemapEncoder(void)
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
    case SCREEN_TEXTEDIT:
        actionSet(ACTION_TEXTEDIT_CHANGE, encCnt);
        break;
    default:
        actionSet(ACTION_AUDIO_PARAM_CHANGE, encCnt);
        break;
    }

    if (ACTION_AUDIO_PARAM_CHANGE == action.type) {
        screenSet(SCREEN_AUDIO_PARAM);
        switch (screen) {
        case SCREEN_AUDIO_INPUT:
            action.param.tune = AUDIO_TUNE_GAIN;
            break;
        case SCREEN_SPECTRUM:
            action.param.tune = AUDIO_TUNE_VOLUME;
        default:
            break;
        }
    }
}


static void actionRemapCommon(void)
{
    Screen screen = screenGet();
    AudioProc *aProc = audioGet();

    switch (action.type) {
    case ACTION_STANDBY:
        if (FLAG_SWITCH == action.value) {
            action.value = (SCREEN_STANDBY == screen ? FLAG_OFF : FLAG_ON);
        }
        break;
    case ACTION_PREV:
        switch (screen) {
        case SCREEN_MENU:
            actionSet(ACTION_NAVIGATE, RC_CMD_NAV_UP);
            break;
        default:
            actionSet(ACTION_NAVIGATE, RC_CMD_NAV_LEFT);
            break;
        }
        break;
    case ACTION_NEXT:
        switch (screen) {
        case SCREEN_MENU:
            actionSet(ACTION_NAVIGATE, RC_CMD_NAV_DOWN);
            break;
        default:
            actionSet(ACTION_NAVIGATE, RC_CMD_NAV_RIGHT);
            break;
        }
        break;
    case ACTION_AUDIO_MENU:
        switch (screen) {
        case SCREEN_MENU:
        case SCREEN_TEXTEDIT:
            actionSet(ACTION_NAVIGATE, RC_CMD_NAV_OK);
            break;
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
             ACTION_NAVIGATE != action.type &&
             ACTION_MENU_CHANGE != action.type &&
             ACTION_MENU_SELECT != action.type &&
             ACTION_ENCODER != action.type)) {
        actionSet(ACTION_NONE, 0);
    }
}

void actionUserGet(void)
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

    actionRemapCommon();

    if (ACTION_NAVIGATE == action.type) {
        actionRemapNavigate();
    }

    if (ACTION_ENCODER == action.type) {
        actionRemapEncoder();
    }
}


void actionHandle(bool visible)
{
    Screen screen = screenGet();
    AudioProc *aProc = audioGet();
    Tuner *tuner = tunerGet();
    int8_t stNum = stationGetNum(tuner->status.freq);

    const Layout *lt = layoutGet();
    Canvas *canvas = canvasGet();

    action.visible = visible;
    action.timeout = 0;

    switch (action.type) {
    case ACTION_STANDBY:
        if (action.value == FLAG_OFF) {
            audioReadSettings();
            tunerReadSettings();

            pinsSetStby(true);      // ON via relay
            swTimSetInitHw(500);

            actionSetScreen(SCREEN_TIME, 800);
        } else {
            screenSaveSettings();

            audioSetMute(true);
            audioSetPower(false);

            tunerSetMute(true);
            tunerSetPower(false);

            pinsDeInitAmpI2c();

            pinsSetStby(false);     // OFF via relay

            actionDispExpired(SCREEN_STANDBY);
        }
        break;
    case ACTION_INIT_HW:
        swTimSetInitHw(SW_TIM_OFF);

        pinsInitAmpI2c();

        tunerInit();
        tunerSetPower(true);
        tunerSetVolume(tuner->par.volume);
        tunerSetMute(false);
        tunerSetFreq(tuner->par.freq);

        audioInit();
        audioSetPower(true);
        break;
    case ACTION_DISP_EXPIRED:
        actionDispExpired(screen);
        break;
    case ACTION_DIGIT:
        if (screen == SCREEN_TIME) {
            rtcEditTime(rtcGetMode(), (int8_t)(action.value));
            actionSetScreen(SCREEN_TIME, 5000);
        }
        break;

    case ACTION_AUDIO_MENU:
        if (screen == SCREEN_AUDIO_PARAM) {
            actionNextAudioParam(aProc);
        } else {
            action.param.tune = AUDIO_TUNE_VOLUME;
        }
        actionSetScreen(SCREEN_AUDIO_PARAM, 5000);
        break;

    case ACTION_RTC_MODE:
        if (screen == SCREEN_TIME) {
            rtcChangeMode(+1);
            actionSetScreen(SCREEN_TIME, 15000);
        } else {
            rtcSetMode(RTC_NOEDIT);
            actionSetScreen(SCREEN_TIME, 5000);
        }
        break;
    case ACTION_RTC_CHANGE:
        rtcChangeTime(rtcGetMode(), (int8_t)(action.value));
        actionSetScreen(screen, 5000);
        break;
    case ACTION_RTC_SET_HOUR:
    case ACTION_RTC_SET_MIN:
    case ACTION_RTC_SET_SEC:
    case ACTION_RTC_SET_DATE:
    case ACTION_RTC_SET_MONTH:
    case ACTION_RTC_SET_YEAR:
        rtcSetTime((int8_t)(action.type - ACTION_RTC_SET_HOUR), (int8_t)(action.value));
        actionSetScreen(screen, 5000);
        break;

    case ACTION_AUDIO_INPUT:
        if (screen == SCREEN_AUDIO_INPUT) {
            actionNextAudioInput(aProc);
            audioSetInput(action.param.input);
        } else {
            action.param.input = aProc->par.input;
        }
        actionSetScreen(SCREEN_AUDIO_INPUT, 5000);
        break;
    case ACTION_AUDIO_PARAM_CHANGE:
        audioChangeTune(action.param.tune, (int8_t)(action.value));
        actionSetScreen(SCREEN_AUDIO_PARAM, 5000);
        break;

    case ACTION_AUDIO_MUTE:
        audioSetMute(action.value);
        // TODO: handle screen
        break;

    case ACTION_TUNER_PREV:
        tunerMove(TUNER_DIR_DOWN);
        actionSetScreen(SCREEN_TUNER, 5000);
        break;
    case ACTION_TUNER_NEXT:
        tunerMove(TUNER_DIR_UP);
        actionSetScreen(SCREEN_TUNER, 5000);
        break;

    case ACTION_TUNER_EDIT_NAME:
        glcdSetFont(lt->textEdit.editFont);
        texteditSet(stationGetName(stNum), STATION_NAME_MAX_LEN, STATION_NAME_MAX_SYM);
        action.prevScreen = SCREEN_TUNER;
        actionSetScreen(SCREEN_TEXTEDIT, 10000);
        break;

    case ACTION_TEXTEDIT_CHANGE:
        texteditChange((int8_t)action.value);
        actionSetScreen(SCREEN_TEXTEDIT, 10000);
        break;
    case ACTION_TEXTEDIT_ADD_CHAR:
        texteditAddChar();
        actionSetScreen(SCREEN_TEXTEDIT, 10000);
        break;
    case ACTION_TEXTEDIT_DEL_CHAR:
        texteditDelChar();
        actionSetScreen(SCREEN_TEXTEDIT, 10000);
        break;
    case ACTION_TEXTEDIT_APPLY:
        stationStore(tuner->status.freq, canvas->te.str);
        actionSetScreen(action.prevScreen, 2000);
        break;
    case ACTION_TEXTEDIT_CANCEL:
        actionSetScreen(action.prevScreen, 2000);
        break;

    case ACTION_BR_STBY:
    case ACTION_BR_WORK:
        screenChangeBrighness((BrMode)(action.type - ACTION_BR_STBY), (int8_t)(action.value));
        actionSetScreen(SCREEN_BRIGHTNESS, 5000);
        break;
    case ACTION_MENU_SELECT:
        menuSetActive((MenuIdx)action.value);
        action.param.parent = menuGet()->parent;
        actionSetScreen(SCREEN_MENU, 10000);
        break;
    case ACTION_MENU_CHANGE:
        menuChange((int8_t)action.value);
        action.param.parent = menuGet()->parent;
        actionSetScreen(SCREEN_MENU, 10000);
        break;
    default:
        break;
    }

    if (action.visible) {
        screenSet(action.screen);
        screenSetParam(action.param);
        if (action.timeout) {
            swTimSetDisplay(action.timeout);
        }
    }
}
