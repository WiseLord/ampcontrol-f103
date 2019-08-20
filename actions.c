#include "actions.h"

#include <stddef.h>

#include "audio/audio.h"
#include "gui/canvas.h"
#include "input.h"
#include "karadio.h"
#include "menu.h"
#include "pins.h"
#include "rc.h"
#include "rtc.h"
#include "settings.h"
#include "spectrum.h"
#include "swtimers.h"
#include "tr/labels.h"
#include "tuner/stations.h"
#include "tuner/tuner.h"
#include "usb/usbhid.h"
#include "usb/hidkeys.h"

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

static void actionHandleStby(int16_t value);

static Action action = {ACTION_POWERUP, false, FLAG_ENTER, SCREEN_STANDBY, 0, ACTION_POWERUP};
static Action qaction = {ACTION_NONE, false, 0, SCREEN_STANDBY, 0, ACTION_NONE};

static AmpStatus ampStatus = AMP_STATUS_STBY;

static void actionSet(ActionType type, int16_t value)
{
    action.type = type;
    action.value = value;
}

static void actionSetScreen(ScreenMode screen, int16_t timeout)
{
    action.screen = screen;
    action.timeout = timeout;
}

static void actionDispExpired(ScreenMode scrMode)
{
    AudioProc *aProc = audioGet();

    Screen *screen = screenGet();
    ScreenMode scrDef = screen->def;

    int32_t timer;

    timer = swTimGet(SW_TIM_STBY_TIMER);
    if (timer > 0 && timer < 60 * 1000 + 999) {
        scrDef = SCREEN_STBY_TIMER;
    }
    timer = swTimGet(SW_TIM_SILENCE_TIMER);
    if (timer > 0 && timer < 30 * 1000 + 999) {
        scrDef = SCREEN_SILENCE_TIMER;
    }

    if (aProc->par.mute) {
        scrDef = SCREEN_AUDIO_INPUT;
    }

    screen->iconHint = ICON_EMPTY;

    rtcSetMode(RTC_NOEDIT);
    aProc->tune = (scrDef == SCREEN_AUDIO_INPUT ? AUDIO_TUNE_GAIN : AUDIO_TUNE_VOLUME);

    switch (scrMode) {
    case SCREEN_STANDBY:
    case SCREEN_MENU:
        actionSetScreen(SCREEN_STANDBY, 1000); // TODO: Return to parent screen caused menu
        break;
    default:
        actionSetScreen(scrDef, 1000);
        break;
    }
}

static void actionResetSilenceTimer(void)
{
    int16_t silenceTimer = settingsGet(PARAM_SYSTEM_SIL_TIM);

    if (silenceTimer) {
        swTimSet(SW_TIM_SILENCE_TIMER, 1000 * 60 * silenceTimer + 999);
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
            actionSet(ACTION_STANDBY, FLAG_ENTER);
        } else {
            actionSet(ACTION_MENU_SELECT, (int16_t)menu->parent);
        }
        break;
    case RC_CMD_NAV_UP:
    case RC_CMD_CHAN_PREV:
        actionSet(ACTION_MENU_CHANGE, -1);
        break;
    case RC_CMD_NAV_DOWN:
    case RC_CMD_CHAN_NEXT:
        actionSet(ACTION_MENU_CHANGE, +1);
        break;
    }
}

static void actionNavigateTextEdit(RcCmd cmd)
{
    switch (cmd) {
    case RC_CMD_NAV_OK:
        action.type = ACTION_TEXTEDIT_APPLY;
        break;
    case RC_CMD_NAV_BACK:
        action.type = ACTION_TEXTEDIT_CANCEL;
        break;
    case RC_CMD_NAV_RIGHT:
    case RC_CMD_CHAN_NEXT:
        action.type = ACTION_TEXTEDIT_ADD_CHAR;
        break;
    case RC_CMD_NAV_LEFT:
    case RC_CMD_CHAN_PREV:
        action.type = ACTION_TEXTEDIT_DEL_CHAR;
        break;
    case RC_CMD_NAV_UP:
        actionSet(ACTION_ENCODER, -1);
        break;
    case RC_CMD_NAV_DOWN:
        actionSet(ACTION_ENCODER, +1);
        break;
    }
}

static void actionNavigateCommon(RcCmd cmd)
{
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];
    Screen *screen = screenGet();

    switch (cmd) {
    case RC_CMD_NAV_OK:
        action.type = ACTION_OPEN_MENU;
        break;
    case RC_CMD_NAV_BACK:
        switch (inType) {
        case IN_PC:
            screen->iconHint = ICON_PLAY_PAUSE;
            actionSet(ACTION_MEDIA, HIDMEDIAKEY_PLAY);
            break;
        default:
            actionSet(ACTION_DISP_EXPIRED, 0);
            break;
        }
        break;
    case RC_CMD_NAV_RIGHT:
        screen->iconHint = ICON_FFWD;
        actionSet(ACTION_MEDIA, HIDMEDIAKEY_FFWD);
        break;
    case RC_CMD_NAV_LEFT:
        screen->iconHint = ICON_REWIND;
        actionSet(ACTION_MEDIA, HIDMEDIAKEY_REWIND);
        break;
    case RC_CMD_NAV_UP:
    case RC_CMD_CHAN_NEXT:
        actionSet(ACTION_MEDIA, HIDMEDIAKEY_NEXT_TRACK);
        break;
    case RC_CMD_NAV_DOWN:
    case RC_CMD_CHAN_PREV:
        actionSet(ACTION_MEDIA, HIDMEDIAKEY_PREV_TRACK);
        break;
    }
}

static void actionNextAudioParam(AudioProc *aProc)
{
    do {
        aProc->tune++;
        if (aProc->tune >= AUDIO_TUNE_END)
            aProc->tune = AUDIO_TUNE_VOLUME;
    } while (aProc->par.item[aProc->tune].grid == NULL && aProc->tune != AUDIO_TUNE_VOLUME);
}

static uint8_t actionGetNextAudioInput(AudioProc *aProc)
{
    uint8_t ret = aProc->par.input + 1;

    if (ret >= aProc->par.inCnt) {
        ret = 0;
    }

    return  ret;
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

static bool isRemoteCmdRepeatable(RcCmd cmd)
{
    ScreenMode screen = screenGetMode();
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    switch (cmd) {
    case RC_CMD_VOL_UP:
    case RC_CMD_VOL_DOWN:
        return true;
    case RC_CMD_NAV_UP:
    case RC_CMD_NAV_DOWN:
        switch (screen) {
        case SCREEN_AUDIO_PARAM:
            return true;
        }
        break;
    case RC_CMD_NAV_LEFT:
    case RC_CMD_NAV_RIGHT:
        switch (screen) {
        case SCREEN_AUDIO_INPUT:
            if (inType == IN_TUNER) {
                return true;
            }
        }
        break;
    }

    return false;
}

static void actionGetRemote(void)
{
    RcData rcData = rcRead(true);
    static RcCmd cmdPrev = RC_CMD_END;

    if (rcData.ready) {
        swTimSet(SW_TIM_RC_NOACION, 200);

        RcCmd cmd = rcGetCmd(&rcData);
        int32_t repTime = swTimGet(SW_TIM_RC_REPEAT);

        if (cmd != cmdPrev) {
            actionSet(ACTION_REMOTE, (int16_t)cmd);
            swTimSet(SW_TIM_RC_REPEAT, 1000);
            cmdPrev = cmd;
        } else {
            if (isRemoteCmdRepeatable(cmd)) {
                if (repTime < 500) {
                    actionSet(ACTION_REMOTE, (int16_t)cmd);
                }
            } else {
                if (repTime == 0) {
                    actionSet(ACTION_REMOTE, (int16_t)cmd);
                    swTimSet(SW_TIM_RC_REPEAT, 1000);
                }
            }
        }
    } else {
        if (swTimGet(SW_TIM_RC_NOACION) == 0) {
            swTimSet(SW_TIM_RC_NOACION, SW_TIM_OFF);
            swTimSet(SW_TIM_RC_REPEAT, 0);
            cmdPrev = RC_CMD_END;
        }
    }
}

static void stbyTimerChange(void)
{
    int32_t stbyTimer = swTimGet(SW_TIM_STBY_TIMER);

    static const uint8_t stbyTimeMin[] = {
        2, 5, 10, 20, 40, 60, 90, 120, 180, 240,
    };

    for (uint8_t i = 0; i < sizeof (stbyTimeMin) / sizeof (stbyTimeMin[0]); i++) {
        int32_t stbyTime = 1000 * 60 * stbyTimeMin[i];
        if (stbyTimer < stbyTime) {
            swTimSet(SW_TIM_STBY_TIMER, stbyTime + 999);
            break;
        }
        if (i == sizeof (stbyTimeMin) / sizeof (stbyTimeMin[0]) - 1) {
            swTimSet(SW_TIM_STBY_TIMER, SW_TIM_OFF);
        }
    }
}

static void spModeChange(Spectrum *sp)
{
    if (++sp->mode >= (sp->peaks ? SP_MODE_END : SP_MODE_WATERFALL)) {
        sp->mode = SP_MODE_STEREO;
        sp->peaks = !sp->peaks;
        settingsStore(PARAM_SPECTRUM_PEAKS, sp->peaks);
    }
    screenToClear();
    settingsStore(PARAM_SPECTRUM_MODE, sp->mode);
}

static void scrDefChange(Screen *screen)
{
    switch (screen->def) {
    case SCREEN_SPECTRUM:
        screen->def = SCREEN_AUDIO_INPUT;
        break;
    case SCREEN_AUDIO_INPUT:
        screen->def = SCREEN_TIME;
        break;
    default:
        screen->def = SCREEN_SPECTRUM;
        break;
    }
}

static void actionGetTimers(void)
{
    if (swTimGet(SW_TIM_DISPLAY) == 0) {
        actionSet(ACTION_DISP_EXPIRED, 0);
    } else if (swTimGet(SW_TIM_INIT_HW) == 0) {
        actionSet(ACTION_INIT_HW, 0);
    } else if (swTimGet(SW_TIM_INIT_SW) == 0) {
        actionSet(ACTION_INIT_SW, 0);
    } else if (swTimGet(SW_TIM_STBY_TIMER) == 0) {
        actionSet(ACTION_STANDBY, FLAG_ENTER);
    } else if (swTimGet(SW_TIM_SILENCE_TIMER) == 0) {
        actionSet(ACTION_STANDBY, FLAG_ENTER);
    } else if (swTimGet(SW_TIM_RTC_INIT) == 0) {
        actionSet(ACTION_INIT_RTC, 0);
    }
}

static void actionRemapBtnShort(void)
{
    switch (action.value) {
    case BTN_D0:
        actionSet(ACTION_STANDBY, FLAG_SWITCH);
        break;
    case BTN_D1:
        actionSet(ACTION_AUDIO_INPUT, FLAG_NEXT);
        break;
    case BTN_D2:
        actionSet(ACTION_NAVIGATE, RC_CMD_NAV_BACK);
        break;
    case BTN_D3:
        actionSet(ACTION_NAVIGATE, RC_CMD_CHAN_PREV);
        break;
    case BTN_D4:
        actionSet(ACTION_NAVIGATE, RC_CMD_CHAN_NEXT);
        break;
    case BTN_D5:
        actionSet(ACTION_NAVIGATE, RC_CMD_NAV_OK);
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
    ScreenMode screen = screenGetMode();
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    switch (action.value) {
    case BTN_D0:
        break;
    case BTN_D1:
        actionSet(ACTION_RTC_MODE, 0);
        break;
    case BTN_D2:
        switch (inType) {
        case IN_TUNER:
            if (screen == SCREEN_TEXTEDIT) {
                action.type = ACTION_TUNER_DEL_STATION;
            } else {
                action.type = ACTION_TUNER_EDIT_NAME;
            }
            break;
        default:
            break;
        }
        break;
    case BTN_D3:
        switch (inType) {
        case IN_TUNER:
            actionSet(ACTION_MEDIA, HIDMEDIAKEY_REWIND);
            break;
        default:
            actionSet(ACTION_NAVIGATE, RC_CMD_CHAN_PREV);
            break;
        }
        break;
    case BTN_D4:
        switch (inType) {
        case IN_TUNER:
            actionSet(ACTION_MEDIA, HIDMEDIAKEY_FFWD);
            break;
        default:
            actionSet(ACTION_NAVIGATE, RC_CMD_CHAN_NEXT);
            break;
        }
        break;
    case BTN_D5:
        switch (screen) {
        case SCREEN_TEXTEDIT:
            action.type = ACTION_OPEN_MENU;
            break;
        case SCREEN_STANDBY:
            actionSet(ACTION_MENU_SELECT, MENU_SETUP_SYSTEM);
            break;
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
    Screen *screen = screenGet();
    ScreenMode scrMode = screen->mode;

    AudioProc *aProc = audioGet();

    if (scrMode == SCREEN_MENU) {
        Menu *menu = menuGet();
        if ((menu->parent == MENU_SETUP_RC) && (menu->selected)) {
            actionSet(ACTION_MENU_CHANGE, 0);
            return;
        }
    }

    if (SCREEN_STANDBY == scrMode &&
        action.value == RC_CMD_MENU) {
        actionSet(ACTION_MENU_SELECT, MENU_SETUP_SYSTEM);
        return;
    }

    if (SCREEN_STANDBY == scrMode &&
        action.value != RC_CMD_STBY_SWITCH)
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
        action.type = ACTION_OPEN_MENU;
        break;

    case RC_CMD_CHAN_NEXT:
        actionSet(ACTION_MEDIA, HIDMEDIAKEY_NEXT_TRACK);
        break;
    case RC_CMD_CHAN_PREV:
        actionSet(ACTION_MEDIA, HIDMEDIAKEY_PREV_TRACK);
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
        actionSet(ACTION_AUDIO_INPUT, FLAG_NEXT);
        break;

    case RC_CMD_NAV_OK:
    case RC_CMD_NAV_BACK:
    case RC_CMD_NAV_RIGHT:
    case RC_CMD_NAV_UP:
    case RC_CMD_NAV_LEFT:
    case RC_CMD_NAV_DOWN:
        actionSet(ACTION_NAVIGATE, action.value);
        break;

    case RC_CMD_BASS_UP:
        screenSetMode(SCREEN_AUDIO_PARAM);
        if (aProc->tune != AUDIO_TUNE_BASS) {
            screenToClear();
        }
        aProc->tune = AUDIO_TUNE_BASS;
        actionSet(ACTION_ENCODER, +1);
        break;
    case RC_CMD_BASS_DOWN:
        screenSetMode(SCREEN_AUDIO_PARAM);
        if (aProc->tune != AUDIO_TUNE_BASS) {
            screenToClear();
        }
        aProc->tune = AUDIO_TUNE_BASS;
        actionSet(ACTION_ENCODER, -1);
        break;
    case RC_CMD_MIDDLE_UP:
        screenSetMode(SCREEN_AUDIO_PARAM);
        if (aProc->tune != AUDIO_TUNE_MIDDLE) {
            screenToClear();
        }
        aProc->tune = AUDIO_TUNE_MIDDLE;
        actionSet(ACTION_ENCODER, +1);
        break;
    case RC_CMD_MIDDLE_DOWN:
        screenSetMode(SCREEN_AUDIO_PARAM);
        if (aProc->tune != AUDIO_TUNE_MIDDLE) {
            screenToClear();
        }
        aProc->tune = AUDIO_TUNE_MIDDLE;
        actionSet(ACTION_ENCODER, -1);
        break;
    case RC_CMD_TREBLE_UP:
        screenSetMode(SCREEN_AUDIO_PARAM);
        if (aProc->tune != AUDIO_TUNE_TREBLE) {
            screenToClear();
        }
        aProc->tune = AUDIO_TUNE_TREBLE;
        actionSet(ACTION_ENCODER, +1);
        break;
    case RC_CMD_TREBLE_DOWN:
        screenSetMode(SCREEN_AUDIO_PARAM);
        if (aProc->tune != AUDIO_TUNE_TREBLE) {
            screenToClear();
        }
        aProc->tune = AUDIO_TUNE_TREBLE;
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
        actionSet(ACTION_RTC_MODE, 0);
        break;

    case RC_CMD_STOP:
        screen->iconHint = ICON_STOP;
        actionSet(ACTION_MEDIA, HIDMEDIAKEY_STOP);
        break;
    case RC_CMD_PLAY_PAUSE:
        screen->iconHint = ICON_PLAY_PAUSE;
        actionSet(ACTION_MEDIA, HIDMEDIAKEY_PLAY);
        break;
    case RC_CMD_REW:
        screen->iconHint = ICON_REWIND;
        actionSet(ACTION_MEDIA, HIDMEDIAKEY_REWIND);
        break;
    case RC_CMD_FWD:
        screen->iconHint = ICON_FFWD;
        actionSet(ACTION_MEDIA, HIDMEDIAKEY_FFWD);
        break;
    case RC_CMD_TIMER:
        actionSet(ACTION_TIMER, 0);
        break;
    case RC_CMD_SP_MODE:
        actionSet(ACTION_SP_MODE, 0);
        break;
    case RC_CMD_SCR_DEF:
        actionSet(ACTION_SCR_DEF, 0);
        break;
    default:
        break;
    }
}

static void actionRemapNavigate(void)
{
    ScreenMode screen = screenGetMode();

    switch (screen) {
    case SCREEN_MENU:
        actionNavigateMenu((RcCmd)action.value);
        break;
    case SCREEN_TEXTEDIT:
        actionNavigateTextEdit((RcCmd)action.value);
        break;
    default:
        actionNavigateCommon((RcCmd)action.value);
        break;
    }
}

static void actionRemapEncoder(void)
{
    ScreenMode screen = screenGetMode();
    AudioProc *aProc = audioGet();

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
        screenSetMode(SCREEN_AUDIO_PARAM);
        switch (screen) {
        case SCREEN_SPECTRUM:
        case SCREEN_AUDIO_FLAG:
        case SCREEN_AUDIO_INPUT:
            aProc->tune = AUDIO_TUNE_VOLUME;
            break;
        default:
            break;
        }
    }
}

static void actionRemapCommon(void)
{
    ScreenMode screen = screenGetMode();
    AudioProc *aProc = audioGet();

    switch (action.type) {
    case ACTION_STANDBY:
        if (FLAG_SWITCH == action.value) {
            action.value = (ampStatus ? FLAG_ENTER : FLAG_EXIT);
        }
        break;
    case ACTION_OPEN_MENU:
        switch (screen) {
        case SCREEN_TEXTEDIT:
            action.type = ACTION_TEXTEDIT_APPLY;
            break;
        case SCREEN_MENU:
            actionSet(ACTION_NAVIGATE, RC_CMD_NAV_OK);
            break;
        }
        break;
    case ACTION_AUDIO_MUTE:
        if (FLAG_SWITCH == action.value) {
            action.value = !aProc->par.mute;
        }
        break;
    case ACTION_AUDIO_LOUDNESS:
        if (FLAG_SWITCH == action.value) {
            action.value = !aProc->par.loudness;
        }
        break;
    case ACTION_AUDIO_SURROUND:
        if (FLAG_SWITCH == action.value) {
            action.value = !aProc->par.surround;
        }
        break;
    case ACTION_AUDIO_EFFECT3D:
        if (FLAG_SWITCH == action.value) {
            action.value = !aProc->par.effect3d;
        }
        break;
    case ACTION_AUDIO_BYPASS:
        if (FLAG_SWITCH == action.value) {
            action.value = !aProc->par.bypass;
        }
        break;
    default:
        break;
    }

    if (SCREEN_STANDBY == screen &&
        (ACTION_STANDBY != action.type &&
         ACTION_REMOTE != action.type &&
         ACTION_INIT_RTC != action.type &&
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

static void actionHandleStby(int16_t value)
{
    if (value == FLAG_EXIT) {
        if (!ampStatus) {
            audioReadSettings();
            tunerReadSettings();

            pinsSetStby(false);     // ON via relay
            swTimSet(SW_TIM_INIT_HW, 500);

            ampStatus = AMP_STATUS_INIT;

            actionSetScreen(SCREEN_TIME, 800);
        }
    } else {
        screenSaveSettings();

        audioSetMute(true);
        audioSetPower(false);

        tunerSetMute(true);
        tunerSetPower(false);

        karadioSetEnabled(false);

        pinsDeInitAmpI2c();

        pinsSetStby(true);      // OFF via relay

        ampStatus = AMP_STATUS_STBY;

        swTimSet(SW_TIM_STBY_TIMER, SW_TIM_OFF);
        swTimSet(SW_TIM_SILENCE_TIMER, SW_TIM_OFF);
        swTimSet(SW_TIM_INIT_HW, SW_TIM_OFF);
        swTimSet(SW_TIM_INIT_SW, SW_TIM_OFF);
        swTimSet(SW_TIM_INPUT_POLL, SW_TIM_OFF);

        actionDispExpired(SCREEN_STANDBY);
    }
}

static void actionDequeue(void)
{
    action.type = qaction.type;
    action.value = qaction.value;

    qaction.type = ACTION_NONE;
    qaction.value = 0;
}

void actionQueue(ActionType type, int16_t value)
{
    qaction.type = type;
    qaction.value = value;
}

void actionUserGet(void)
{
    actionSet(ACTION_NONE, 0);

    actionDequeue();

    if (ACTION_NONE == action.type) {
        actionGetButtons();
    }

    if (ACTION_NONE == action.type) {
        actionGetEncoder();
    }

    if (ACTION_NONE == action.type) {
        actionGetRemote();
    }

    if (ACTION_NONE == action.type) {
        ScreenMode screen = screenGetMode();

        if (screen == SCREEN_STANDBY && rtcCheckAlarm()) {
            actionSet(ACTION_STANDBY, FLAG_EXIT);
        }
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
    Screen *screen = screenGet();
    ScreenMode scrMode = screen->mode;

    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];
    Tuner *tuner = tunerGet();
    int8_t stNum = stationGetNum(tuner->status.freq);

    Canvas *canvas = canvasGet();
    const Layout *lt = canvas->layout;

    Spectrum *sp = spGet();

    action.visible = visible;
    action.timeout = 0;

    switch (action.type) {
    case ACTION_POWERUP:
        pinsSetMute(true);
        pinsSetStby(true);

        swTimSet(SW_TIM_RTC_INIT, 500);
        break;
    case ACTION_STANDBY:
        actionHandleStby(action.value);
        break;
    case ACTION_INIT_HW:
        swTimSet(SW_TIM_INIT_HW, SW_TIM_OFF);

        pinsInitAmpI2c();
        pinsSetStby(false);

        tunerInit();
        audioInit();

        ampStatus = AMP_STATUS_ACTIVE;

        swTimSet(SW_TIM_INPUT_POLL, 800);
        swTimSet(SW_TIM_INIT_SW, 300);
        break;
    case ACTION_INIT_RTC:
        rtcInit();
        break;
    case ACTION_INIT_SW:
        swTimSet(SW_TIM_INIT_SW, SW_TIM_OFF);

        tunerSetPower(true);
        tunerSetVolume(tuner->par.volume);
        tunerSetMute(false);
        tunerSetFreq(tuner->par.freq);

        audioSetPower(true);
        actionResetSilenceTimer();

        karadioSetEnabled(inType == IN_KARADIO);
        break;
    case ACTION_DISP_EXPIRED:
        actionDispExpired(scrMode);
        break;
    case ACTION_DIGIT:
        if (scrMode == SCREEN_TIME) {
            rtcEditTime(rtcGetMode(), (int8_t)(action.value));
            actionSetScreen(SCREEN_TIME, 5000);
        }
        break;

    case ACTION_MEDIA:
        switch (inType) {
        case IN_TUNER:
            switch (action.value) {
            case HIDMEDIAKEY_PREV_TRACK:
                tunerMove(TUNER_DIR_DOWN);
                screen->iconHint = ICON_PREV_TRACK;
                break;
            case HIDMEDIAKEY_NEXT_TRACK:
                tunerMove(TUNER_DIR_UP);
                screen->iconHint = ICON_NEXT_TRACK;
                break;
            case HIDMEDIAKEY_REWIND:
                tunerSeek(TUNER_DIR_DOWN);
                break;
            case HIDMEDIAKEY_FFWD:
                tunerSeek(TUNER_DIR_UP);
                break;
            }
            actionSetScreen(SCREEN_AUDIO_INPUT, 3000);
            break;
        case IN_PC:
            usbHidSendMediaKey((HidKey)action.value);
            actionSetScreen(SCREEN_AUDIO_INPUT, 1000);
            break;
        case IN_KARADIO:
            karadioSendMediaCmd((HidKey)action.value);
            actionSetScreen(SCREEN_AUDIO_INPUT, 1000);
            break;
        }
        break;

    case ACTION_OPEN_MENU:
        if (scrMode == SCREEN_AUDIO_PARAM) {
            screenToClear();
            actionNextAudioParam(aProc);
        } else {
            aProc->tune = AUDIO_TUNE_VOLUME;
        }
        actionSetScreen(SCREEN_AUDIO_PARAM, 5000);
        break;

    case ACTION_RTC_MODE:
        if (scrMode == SCREEN_TIME) {
            rtcChangeMode(+1);
        } else {
            rtcSetMode(RTC_NOEDIT);
        }
        actionSetScreen(SCREEN_TIME, rtcGetMode() == RTC_NOEDIT ? 5000 : 30000);
        break;
    case ACTION_RTC_CHANGE:
        if (action.value < 0) {
            rtcChangeTime(rtcGetMode(), DIRECTION_DOWN);
        } else if (action.value > 0) {
            rtcChangeTime(rtcGetMode(), DIRECTION_UP);
        }
        actionSetScreen(scrMode, 5000);
        break;
    case ACTION_RTC_SET_HOUR:
    case ACTION_RTC_SET_MIN:
    case ACTION_RTC_SET_SEC:
    case ACTION_RTC_SET_DATE:
    case ACTION_RTC_SET_MONTH:
    case ACTION_RTC_SET_YEAR:
        rtcSetTime((RtcMode)(action.type - ACTION_RTC_SET_HOUR), (int8_t)(action.value));
        actionSetScreen(scrMode, 5000);
        break;

    case ACTION_AUDIO_INPUT:
        if (scrMode == SCREEN_AUDIO_INPUT) {
            audioSetInput(actionGetNextAudioInput(aProc));
            inType = aProc->par.inType[aProc->par.input];
        }

        karadioSetEnabled(inType == IN_KARADIO);

        screenToClear();
        screen->iconHint = ICON_EMPTY;
        actionSetScreen(SCREEN_AUDIO_INPUT, 5000);
        break;
    case ACTION_AUDIO_PARAM_CHANGE:
        audioChangeTune(aProc->tune, (int8_t)(action.value));
        actionSetScreen(SCREEN_AUDIO_PARAM, 3000);
        break;

    case ACTION_AUDIO_MUTE:
        audioSetMute(action.value);
        if (aProc->tune != AUDIO_FLAG_MUTE) {
            screenToClear();
        }
        aProc->tune = AUDIO_FLAG_MUTE;
        actionSetScreen(SCREEN_AUDIO_FLAG, 3000);
        break;
    case ACTION_AUDIO_LOUDNESS:
        audioSetLoudness(action.value);
        if (aProc->tune != AUDIO_FLAG_LOUDNESS) {
            screenToClear();
        }
        aProc->tune = AUDIO_FLAG_LOUDNESS;
        actionSetScreen(SCREEN_AUDIO_FLAG, 3000);
        break;
    case ACTION_AUDIO_SURROUND:
        audioSetSurround(action.value);
        if (aProc->tune != AUDIO_FLAG_SURROUND) {
            screenToClear();
        }
        aProc->tune = AUDIO_FLAG_SURROUND;
        actionSetScreen(SCREEN_AUDIO_FLAG, 3000);
        break;
    case ACTION_AUDIO_EFFECT3D:
        audioSetEffect3D(action.value);
        if (aProc->tune != AUDIO_FLAG_EFFECT3D) {
            screenToClear();
        }
        aProc->tune = AUDIO_FLAG_EFFECT3D;
        actionSetScreen(SCREEN_AUDIO_FLAG, 3000);
        break;
    case ACTION_AUDIO_BYPASS:
        audioSetBypass(action.value);
        if (aProc->tune != AUDIO_FLAG_BYPASS) {
            screenToClear();
        }
        aProc->tune = AUDIO_FLAG_BYPASS;
        actionSetScreen(SCREEN_AUDIO_FLAG, 3000);
        break;

    case ACTION_TUNER_EDIT_NAME:
        glcdSetFont(lt->textEdit.editFont);
        canvas->te.name = labelsGet(LABEL_TUNER_FM_STATION_NAME);
        textEditSet(&canvas->te, stationGetName(stNum), STATION_NAME_MAX_LEN, STATION_NAME_MAX_SYM);
        action.prevScreen = SCREEN_AUDIO_INPUT;
        actionSetScreen(SCREEN_TEXTEDIT, 10000);
        break;
    case ACTION_TUNER_DEL_STATION:
        stationRemove(tuner->status.freq);
        actionSetScreen(action.prevScreen, 2000);
        break;

    case ACTION_TEXTEDIT_CHANGE:
        textEditChange(&canvas->te, (int8_t)action.value);
        actionSetScreen(SCREEN_TEXTEDIT, 10000);
        break;
    case ACTION_TEXTEDIT_ADD_CHAR:
        textEditAddChar(&canvas->te);
        actionSetScreen(SCREEN_TEXTEDIT, 10000);
        break;
    case ACTION_TEXTEDIT_DEL_CHAR:
        textEditDelChar(&canvas->te);
        actionSetScreen(SCREEN_TEXTEDIT, 10000);
        break;
    case ACTION_TEXTEDIT_APPLY:
        stationStore(tuner->status.freq, canvas->te.str);
        actionSetScreen(action.prevScreen, 2000);
        break;
    case ACTION_TEXTEDIT_CANCEL:
        actionSetScreen(action.prevScreen, 2000);
        break;

    case ACTION_MENU_SELECT: {
        MenuIdx parent = menuGet()->parent;
        menuSetActive((MenuIdx)action.value);
        if (parent != menuGet()->parent) {
            screenToClear();
        }
        actionSetScreen(SCREEN_MENU, 10000);
        break;
    }
    case ACTION_MENU_CHANGE:
        menuChange((int8_t)action.value);
        actionSetScreen(SCREEN_MENU, 10000);
        break;

    case ACTION_TIMER:
        if (scrMode == SCREEN_STBY_TIMER) {
            stbyTimerChange();
        }
        actionSetScreen(SCREEN_STBY_TIMER, 5000);
        break;
    case ACTION_SP_MODE:
        if (scrMode == SCREEN_SPECTRUM) {
            spModeChange(sp);
        }
        actionSetScreen(SCREEN_SPECTRUM, 3000);
        break;
    case ACTION_SCR_DEF:
        rtcSetMode(RTC_NOEDIT);
        if (scrMode == screen->def) {
            scrDefChange(screen);
        }
        actionSetScreen(screen->def, 3000);
        break;
    default:
        break;
    }

    // Reset silence timer on any user action
    if (action.type != ACTION_NONE && action.type != ACTION_DISP_EXPIRED) {
        actionResetSilenceTimer();
    }

    // Reset silence timer on signal
    if (scrMode != SCREEN_STANDBY) {
        Spectrum *spectrum = spGet();
        if (spectrum->data[SP_CHAN_LEFT].max > 128 ||
            spectrum->data[SP_CHAN_RIGHT].max > 128) {
            actionResetSilenceTimer();
        }
    }

    if (action.visible) {
        screenSetMode(action.screen);
        if (action.timeout > 0) {
            swTimSet(SW_TIM_DISPLAY, action.timeout);
        }
    }
}

AmpStatus actionGetAmpStatus(void)
{
    return ampStatus;
}
