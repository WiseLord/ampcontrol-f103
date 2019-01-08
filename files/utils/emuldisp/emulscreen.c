#include "emulscreen.h"

#include "../../../screen.h"

#include "../../../canvas/canvas.h"
#include "../../../display/glcd.h"
#include "../../../tr/labels.h"
#include "../../../eemul.h"
#include "../../../menu.h"
#include "../../../spectrum.h"

#include <time.h>

static Screen screen = EMUL_SCREEN;

static ScreenParam scrPar;
static Canvas *canvas;
static Spectrum spectrum;

void ltEmulInit(Canvas *value)
{
#if EMUL_DISP_WIDTH == 160 && EMUL_DISP_HEIGHT == 128
    lt160x128Init(value);
#elif EMUL_DISP_WIDTH == 176 && EMUL_DISP_HEIGHT == 132
    lt176x132Init(value);
#elif EMUL_DISP_WIDTH == 220 && EMUL_DISP_HEIGHT == 176
    lt220x176Init(value);
#elif EMUL_DISP_WIDTH == 320 && EMUL_DISP_HEIGHT == 240
    lt320x240Init(value);
#elif EMUL_DISP_WIDTH == 400 && EMUL_DISP_HEIGHT == 240
    lt400x240Init(value);
#elif EMUL_DISP_WIDTH == 480 && EMUL_DISP_HEIGHT == 320
    lt480x320Init(value);
#elif EMUL_DISP_WIDTH == 128 && EMUL_DISP_HEIGHT == 64
    lt128x64Init(value);
#else
#error "No such canvas"
#endif
    canvas = value;
}

void screenShow(void)
{
    switch (screen) {
    case SCREEN_STANDBY:
    case SCREEN_TIME:
        screenShowTime(true);
        break;
    case SCREEN_SPECTRUM:
        screenShowSpectrum(true);
        break;
    case SCREEN_BRIGHTNESS:
        screenShowBrightness(true);
        break;
    case SCREEN_AUDIO_INPUT:
        screenShowInput(true);
        break;
    case SCREEN_AUDIO_PARAM:
        screenShowAudioParam(true);
        break;
    case SCREEN_TUNER:
        screenShowTuner(true);
        break;
    case SCREEN_MENU:
        screenShowMenu();
        break;
    default:
        break;
    }
}

void screenShowTime(bool clear)
{
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);

    RTC_type rtc;
    rtc.hour = (int8_t)lt->tm_hour;
    rtc.min = (int8_t)lt->tm_min;
    rtc.sec = (int8_t)lt->tm_sec;

    rtc.date = (int8_t)lt->tm_mday;
    rtc.month = (int8_t)lt->tm_mon;
    rtc.year = (int8_t)lt->tm_year - 100;

    rtc.wday = (int8_t)lt->tm_wday;

    rtc.etm = RTC_HOUR;

    canvasShowTime(clear, &rtc);
}

void screenShowSpectrum(bool clear)
{
    for (uint8_t i = 0; i < FFT_SIZE / 8; i++) {
        spectrum.chan[SP_CHAN_LEFT].show[i] = i / 2;
        spectrum.chan[SP_CHAN_RIGHT].show[i] = i / 2;
        spectrum.chan[SP_CHAN_LEFT].peak[i] = (uint8_t)N_DB - i;
        spectrum.chan[SP_CHAN_RIGHT].peak[i] = (uint8_t)N_DB - i;
    }

    canvasShowSpectrum(clear, &spectrum);
}

void screenShowBrightness(bool clear)
{
    DispParam dp;

    dp.label = labelsGet(LABEL_BRIGNTNESS);
    dp.value = 14;
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
    AudioTune aTune = AUDIO_TUNE_VOLUME;

    uint8_t input = 0;
    int8_t value = -24;

    if (aTune >= AUDIO_TUNE_END)
        aTune = AUDIO_TUNE_VOLUME;

    DispParam dp;
    if (aTune == AUDIO_TUNE_GAIN) {
        InputType inType = (uint8_t)eeReadI(EE_AUDIO_IN0 + aProc->par.input, IN_TUNER + aProc->par.input);
        dp.label = labelsGet(LABEL_IN_TUNER + inType);
        dp.icon = ICON_TUNER + input;
    } else {
        dp.label = labelsGet(LABEL_VOLUME + aTune);
        dp.icon = (uint8_t)(ICON_VOLUME + aTune);
    }
    dp.value = value;

    dp.min = -79;
    dp.max = 0;
    dp.mStep = 1 * 8;

    canvasShowTune(clear, &dp, &spectrum);
}

void screenShowTuner(bool clear)
{
    Tuner *tuner = tunerGet();

    tuner->par.freq = 10120;
    tuner->par.fMin = 8700;
    tuner->par.fMax = 10800;

    canvasShowTuner(clear, tuner, &spectrum);
}

void screenShowMenu(void)
{
    // Menu
    menuSetActive(MENU_TUNER_BASS);
    menuChange(+1);
    menuChange(+1);
    menuChange(+1);
    menuSetActive(MENU_TUNER_STEP);

    canvasShowMenu();
}

Spectrum *spGet(void)
{
    return &spectrum;
}

void inputSetEncRes(int8_t value)
{
    (void)value;
}
