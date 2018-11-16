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
static SpectrumData spData[SP_CHAN_END];

void emulCanvasInit(Canvas **value)
{
#if EMUL_DISP_WIDTH == 160 && EMUL_DISP_HEIGHT == 128
    gc160x128Init(value);
#elif EMUL_DISP_WIDTH == 176 && EMUL_DISP_HEIGHT == 132
    gc176x132Init(value);
#elif EMUL_DISP_WIDTH == 220 && EMUL_DISP_HEIGHT == 176
    gc220x176Init(value);
#elif EMUL_DISP_WIDTH == 320 && EMUL_DISP_HEIGHT == 240
    gc320x240Init(value);
#elif EMUL_DISP_WIDTH == 400 && EMUL_DISP_HEIGHT == 240
    gc400x240Init(value);
#elif EMUL_DISP_WIDTH == 480 && EMUL_DISP_HEIGHT == 320
    gc480x320Init(value);
#elif EMUL_DISP_WIDTH == 128 && EMUL_DISP_HEIGHT == 64
    gm128x64Init(value);
#else
#error "No such canvas"
#endif
    canvas = *value;
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
        screenShowBrightness();
        break;
    case SCREEN_AUDIO_INPUT:
        screenShowInput();
        break;
    case SCREEN_AUDIO_PARAM:
        screenShowAudioParam();
        break;
    case SCREEN_TUNER:
        screenShowTuner();
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
    rtc.hour = lt->tm_hour;
    rtc.min = lt->tm_min;
    rtc.sec = lt->tm_sec;

    rtc.date = lt->tm_mday;
    rtc.month = lt->tm_mon;
    rtc.year = lt->tm_year - 100;

    rtc.wday = lt->tm_wday;

    rtc.etm = RTC_HOUR;

    canvasShowTime(clear, &rtc);
}

void screenShowSpectrum(bool clear)
{
    for (int i = 0; i < FFT_SIZE / 2; i++) {
        spData[SP_CHAN_LEFT].show[i] = i / 2;
        spData[SP_CHAN_RIGHT].show[i] = i / 2;
        spData[SP_CHAN_LEFT].peak[i] = N_DB - i;
        spData[SP_CHAN_RIGHT].peak[i] = N_DB - i;
    }

    canvasShowSpectrum(clear, spData);
}

void screenShowBrightness(void)
{
    DispParam dp;
    const char **txtLabels = labelsGet();

    dp.label = txtLabels[LABEL_BRIGNTNESS];
    dp.value = 14;
    dp.min = LCD_BR_MIN;
    dp.max = LCD_BR_MAX;
    dp.step = 1 * 8;
    dp.icon = ICON_BRIGHTNESS;

    canvasShowTune(&dp);
}

void screenShowInput(void)
{
    scrPar.tune = AUDIO_TUNE_GAIN;
    screenShowAudioParam();
}

void screenShowAudioParam(void)
{
    AudioTune aTune = AUDIO_TUNE_VOLUME;
    uint8_t input = 0;
    int8_t value = -24;

    const char **txtLabels = labelsGet();

    if (aTune >= AUDIO_TUNE_END)
        aTune = AUDIO_TUNE_VOLUME;

    DispParam dp;
    if (aTune == AUDIO_TUNE_GAIN) {
        dp.label = txtLabels[LABEL_GAIN0 + input];
        dp.icon = ICON_TUNER + input;
    } else {
        dp.label = txtLabels[LABEL_VOLUME + aTune];
        dp.icon = ICON_VOLUME + aTune;
    }
    dp.value = value;

    dp.min = -79;
    dp.max = 0;
    dp.step = 1 * 8;

    canvasShowTune(&dp);
}

void screenShowTuner(void)
{
    Tuner *tuner = tunerGet();

    tuner->freq = 10120;
    tuner->par.fMin = 8700;
    tuner->par.fMax = 10800;

    canvasShowTuner(tuner);
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
