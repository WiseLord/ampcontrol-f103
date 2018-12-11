#include "screen.h"

#include "canvas/canvas.h"
#include "display/glcd.h"
#include "tr/labels.h"
#include "actions.h"
#include "eemul.h"
#include "fft.h"
#include "spectrum.h"
#include "timers.h"

static Screen screen = SCREEN_STANDBY;
static Screen screenDefault = SCREEN_SPECTRUM;
static ScreenParam scrPar;

static Spectrum spectrum;

// TODO: Read from backup memory
static int8_t brStby;
static int8_t brWork;

static bool screenCheckClear(void)
{
    bool clear = false;

    static Screen scrPrev = SCREEN_STANDBY;
    static ScreenParam scrParPrev;

    AudioProc *aProc = audioGet();

    // Check if we need to clear screen
    if (screen != scrPrev) {
        clear = true;
        switch (screen) {
        case SCREEN_TIME:
        case SCREEN_STANDBY:
            if (scrPrev == SCREEN_STANDBY || scrPrev == SCREEN_TIME) {
                clear = false;
            }
            break;
        case SCREEN_AUDIO_PARAM:
            if (scrPrev == SCREEN_AUDIO_INPUT && scrPar.tune == AUDIO_TUNE_GAIN) {
                clear = false;
            }
            break;
        case SCREEN_AUDIO_INPUT:
            if (scrPrev == SCREEN_AUDIO_PARAM) {
                if (scrParPrev.tune == AUDIO_TUNE_GAIN) {
                    InputType inType = (uint8_t)eeReadI(EE_AUDIO_IN0 + aProc->par.input, IN_TUNER + aProc->par.input);
                    if (IN_TUNER != inType) {
                        clear = false;
                    }
                }
            }
            break;
        case SCREEN_MENU:
            menuGet()->selected = 0;
            break;
        default:
            break;
        }

        // Enable/disable tuner polling
        if (screen == SCREEN_TUNER) {
            swTimSetTunerPoll(0);
        } else {
            swTimSetTunerPoll(SW_TIM_OFF);
        }

        // Handle standby/work brightness
        if (screen == SCREEN_STANDBY) {
            screenChangeBrighness(ACTION_BR_STBY, 0);
        } else {
            screenChangeBrighness(ACTION_BR_WORK, 0);
        }
    } else {
        switch (screen) {
        case SCREEN_AUDIO_PARAM:
            if (scrPar.tune != scrParPrev.tune) {
                clear = true;
            }
            break;
        case SCREEN_AUDIO_INPUT:
            if (scrPar.input != scrParPrev.input) {
                clear = true;
            }
            break;
        case SCREEN_MENU:
            if (scrPar.parent != scrParPrev.parent) {
                clear = true;
            }
            break;
        default:
            break;
        }
    }

    // Save current screen and screen parameter
    scrPrev = screen;
    scrParPrev = scrPar;

    return clear;
}

void screenReadSettings(void)
{
    brStby = (int8_t)eeReadI(EE_DISPLAY_BR_STBY, 3);
    if (brStby < LCD_BR_MIN) {
        brStby = LCD_BR_MIN;
    } else if (brStby > LCD_BR_MAX) {
        brStby = LCD_BR_MAX;
    }

    brWork = (int8_t)eeReadI(EE_DISPLAY_BR_WORK, LCD_BR_MAX);
    if (brWork < LCD_BR_MIN) {
        brWork = LCD_BR_MIN;
    } else if (brWork > LCD_BR_MAX) {
        brWork = LCD_BR_MAX;
    }
}

void screenSaveSettings(void)
{
    eeUpdate(EE_DISPLAY_BR_STBY, brStby);
    eeUpdate(EE_DISPLAY_BR_WORK, brWork);
}


void screenInit(void)
{
    labelsInit();
    canvasInit();
    canvasClear();
    screenReadSettings();
    dispdrvSetBrightness(brStby);
}

void screenSet(Screen value)
{
    screen = value;
}

Screen screenGet()
{
    return screen;
}


void screenSetParam(ScreenParam param)
{
    scrPar = param;
}


void screenSetDefault(Screen value)
{
    screenDefault = value;
}

Screen screenGetDefault(void)
{
    AudioProc *aProc = audioGet();

    InputType inType = (uint8_t)eeReadI(EE_AUDIO_IN0 + aProc->par.input, IN_TUNER + aProc->par.input);

    if (IN_TUNER == inType)
        return SCREEN_TUNER;

    return screenDefault;
}


int8_t screenGetBrightness(uint8_t mode)
{
    if (mode == ACTION_BR_WORK)
        return brWork;
    else
        return brStby;
}

void screenSetBrightness(uint8_t mode, int8_t value)
{
    if (mode == ACTION_BR_WORK)
        brWork = value;
    else
        brStby = value;

    dispdrvSetBrightness(value);
}

void screenChangeBrighness(uint8_t mode, int8_t diff)
{
    int8_t br = screenGetBrightness(mode);

    br += diff;

    if (br > LCD_BR_MAX)
        br = LCD_BR_MAX;
    if (br < LCD_BR_MIN)
        br = LCD_BR_MIN;

    screenSetBrightness(mode, br);
}

void screenShow(void)
{
    bool clear = screenCheckClear();

    // Get new spectrum data
    if (swTimGetSpConvert() <= 0) {
        swTimSetSpConvert(20);
        spGetADC(spectrum.chan[SP_CHAN_LEFT].raw, spectrum.chan[SP_CHAN_RIGHT].raw);
        spectrum.ready = true;
    }

    if (clear) {
        canvasClear();
        spectrum.redraw = true;
    }

    switch (screen) {
    case SCREEN_STANDBY:
    case SCREEN_TIME:
        screenShowTime(clear);
        break;
    case SCREEN_SPECTRUM:
        screenShowSpectrum(clear);
        break;
    case SCREEN_BRIGHTNESS:
        screenShowBrightness(clear);
        break;
    case SCREEN_AUDIO_INPUT:
        screenShowInput(clear);
        break;
    case SCREEN_AUDIO_PARAM:
        screenShowAudioParam(clear);
        break;
    case SCREEN_TUNER:
        screenShowTuner(clear);
        break;
    case SCREEN_MENU:
        screenShowMenu();
        break;
    default:
        break;
    }

    glcdUpdate();
}

void screenShowTime(bool clear)
{
    RTC_type rtc;
    rtc.etm = rtcGetMode();

    rtcGetTime(&rtc);

    canvasShowTime(clear, &rtc);
}

void screenShowSpectrum(bool clear)
{
    canvasShowSpectrum(clear, &spectrum);
}

void screenShowBrightness(bool clear)
{
    DispParam dp;
    const char **txtLabels = labelsGet();

    dp.label = txtLabels[LABEL_BRIGNTNESS];
    dp.value = screenGetBrightness(ACTION_BR_WORK);
    dp.min = LCD_BR_MIN;
    dp.max = LCD_BR_MAX;
    dp.mStep = 1 * 8;
    dp.icon = ICON_BRIGHTNESS;

    canvasShowTune(clear, &dp, &spectrum);
}

void screenShowInput(bool clear)
{
    scrPar.tune = AUDIO_TUNE_GAIN;
    screenShowAudioParam(clear);
}

void screenShowAudioParam(bool clear)
{
    AudioProc *aProc = audioGet();
    AudioTune aTune = scrPar.tune;
    const char **txtLabels = labelsGet();

    if (aTune >= AUDIO_TUNE_END)
        aTune = AUDIO_TUNE_VOLUME;

    DispParam dp;
    if (aTune == AUDIO_TUNE_GAIN) {
        InputType inType = (uint8_t)eeReadI(EE_AUDIO_IN0 + aProc->par.input, IN_TUNER + aProc->par.input);
        dp.label = txtLabels[LABEL_IN_TUNER + inType];
        dp.icon = (uint8_t)(ICON_TUNER + inType);
    } else {
        dp.label = txtLabels[LABEL_VOLUME + aTune];
        dp.icon = (uint8_t)(ICON_VOLUME + aTune);
    }
    dp.value = aProc->par.item[aTune].value;
    const AudioGrid *grid = aProc->par.item[aTune].grid;

    dp.min = grid ? grid->min : 0;
    dp.max = grid ? grid->max : 0;
    dp.mStep = grid ? grid->mStep : 0;

    canvasShowTune(clear, &dp, &spectrum);
}

void screenShowTuner(bool clear)
{
    Tuner *tuner = tunerGet();

    if (swTimGetTunerPoll() == 0) {
        tunerUpdateStatus();
        swTimSetTunerPoll(100);
    }

    canvasShowTuner(clear, tuner, &spectrum);
}

void screenShowMenu(void)
{
    canvasShowMenu();
}
