#include "screen.h"

#include "canvas/canvas.h"
#include "display/glcd.h"
#include "tr/labels.h"
#include "actions.h"
#include "eemul.h"
#include "fft.h"
#include "menu.h"
#include "spectrum.h"
#include "timers.h"

static Canvas *canvas;

static Screen screen = SCREEN_STANDBY;
static Screen screenDefault = SCREEN_SPECTRUM;
static ScreenParam scrPar;

static SpectrumData spData[SP_CHAN_END];
static uint8_t spReady = 0;

// TODO: Read from backup memory
static int8_t brStby;
static int8_t brWork;

static void improveSpectrum(SpectrumData *sd, uint16_t height)
{
    for (uint8_t i = 0; i < FFT_SIZE / 2; i++) {
        sd->data[i] = height * sd->data[i] / N_DB;

        sd->old_show[i] = sd->show[i];
        if (sd->data[i] < sd->show[i]) {
            if (sd->show[i] >= sd->fall[i]) {
                sd->show[i] -= sd->fall[i];
                sd->fall[i]++;
            } else {
                sd->show[i] = 0;
            }
        }

        if (sd->data[i] > sd->show[i]) {
            sd->show[i] = sd->data[i];
            sd->fall[i] = 1;
        }

        sd->old_peak[i] = sd->peak[i];
        if (sd->peak[i] <= sd->data[i]) {
            sd->peak[i] = sd->data[i] + 1;
        } else {
            if (sd->peak[i]) {
                sd->peak[i]--;
            }
        }
    }
}

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
                    if (aProc->par.input != INPUT_TUNER) {
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
    brStby = eeReadU(EE_BRIGHTNESS_STBY, 3);
    if (brStby < LCD_BR_MIN) {
        brStby = LCD_BR_MIN;
    } else if (brStby > LCD_BR_MAX) {
        brStby = LCD_BR_MAX;
    }

    brWork = eeReadU(EE_BRIGHTNESS_WORK, LCD_BR_MAX);
    if (brWork < LCD_BR_MIN) {
        brWork = LCD_BR_MIN;
    } else if (brWork > LCD_BR_MAX) {
        brWork = LCD_BR_MAX;
    }
}

void screenSaveSettings(void)
{
    eeUpdate(EE_BRIGHTNESS_STBY, brStby);
    eeUpdate(EE_BRIGHTNESS_WORK, brWork);
}


void screenInit(void)
{
    labelsInit();

    Glcd *glcd;

    glcdInit(&glcd);
    canvasInit(&canvas);
    canvas->glcd = glcd;

    screenClear();

    screenReadSettings();

    dispdrvSetBrightness(brStby);
}

void screenClear(void)
{
    glcdDrawRect(0, 0, canvas->width, canvas->height, canvas->color);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(canvas->color);
}

void screenUpdate(void)
{
    glcdUpdate();
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

    if (INPUT_TUNER == aProc->par.input)
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
        spGetADC(spData[SP_CHAN_LEFT].data, spData[SP_CHAN_RIGHT].data);

        spReady = 1;
    }

    if (clear) {
        screenClear();
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

    screenUpdate();
}

void screenShowTime(bool clear)
{
    RTC_type rtc;
    rtc.etm = rtcGetMode();

    rtcGetTime(&rtc);

    if (canvas->showTime) {
        canvas->showTime(clear, &rtc);
    }
}

void screenShowSpectrum(bool clear)
{
    static bool spClear = false;

    if (clear) {
        spClear = true;
    }

    if (spReady) {
        if (canvas->showSpectrum) {
            improveSpectrum(&spData[SP_CHAN_LEFT], canvas->height / 2);
            improveSpectrum(&spData[SP_CHAN_RIGHT], canvas->height / 2);
            canvas->showSpectrum(spClear, spData);
            spClear = false;
        }
        spReady = 0;
    }
}

void screenShowBrightness(void)
{
    DispParam dp;
    const char **txtLabels = labelsGet();

    dp.label = txtLabels[LABEL_BRIGNTNESS];
    dp.value = screenGetBrightness(ACTION_BR_WORK);
    dp.min = LCD_BR_MIN;
    dp.max = LCD_BR_MAX;
    dp.step = 1 * 8;
    dp.icon = ICON_BRIGHTNESS;

    if (canvas->showParam) {
        canvas->showParam(&dp);
    }
}

void screenShowInput(void)
{
    scrPar.tune = AUDIO_TUNE_GAIN;
    screenShowAudioParam();
}

void screenShowAudioParam(void)
{
    AudioProc *aProc = audioGet();
    AudioTune aTune = scrPar.tune;
    const char **txtLabels = labelsGet();

    if (aTune >= AUDIO_TUNE_END)
        aTune = AUDIO_TUNE_VOLUME;

    DispParam dp;
    if (aTune == AUDIO_TUNE_GAIN) {
        dp.label = txtLabels[LABEL_GAIN0 + aProc->par.input];
        dp.icon = ICON_TUNER + aProc->par.input;
    } else {
        dp.label = txtLabels[LABEL_VOLUME + aTune];
        dp.icon = ICON_VOLUME + aTune;
    }
    dp.value = aProc->par.item[aTune].value;
    const AudioGrid *grid = aProc->par.item[aTune].grid;

    dp.min = grid ? grid->min : 0;
    dp.max = grid ? grid->max : 0;
    dp.step = grid ? grid->step : 0;

    if (canvas->showParam) {
        canvas->showParam(&dp);
    }
}

void screenShowTuner(void)
{
    DispTuner dt;
    dt.tuner = tunerGet();

    if (swTimGetTunerPoll() == 0) {
        tunerUpdateStatus();
        swTimSetTunerPoll(100);
    }

    if (canvas->showTuner) {
        canvas->showTuner(&dt);
    }
}

void screenShowMenu(void)
{
    canvasShowMenu();
}
