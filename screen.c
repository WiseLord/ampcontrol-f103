#include "screen.h"

#include "canvas/canvas.h"
#include "display/glcd.h"
#include "tr/labels.h"
#include "actions.h"
#include "eemul.h"
#include "fft.h"
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

static void improveSpectrum(SpectrumData *sd)
{
    for (uint8_t i = 0; i < FFT_SIZE / 2; i++) {
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
            sd->fall[i] = 0;
        }

        if (sd->peak[i] < sd->data[i]) {
            sd->peak[i] = sd->data[i];
        } else {
            if (sd->peak[i]) {
                sd->peak[i]--;
            }
        }
    }
}

static void screenCheckChange(void)
{
    static Screen scrPrev = SCREEN_STANDBY;
    static ScreenParam scrParPrev;

    uint8_t clearFlag = 0;
    AudioProc *aProc = audioProcGet();

    // Check if we need to clear screen
    if (screen != scrPrev) {
        clearFlag = 1;
        switch (screen) {
        case SCREEN_TIME:
        case SCREEN_STANDBY:
            if (scrPrev == SCREEN_STANDBY || scrPrev == SCREEN_TIME) {
                clearFlag = 0;
            }
            break;
        case SCREEN_AUDIO_PARAM:
            if (scrPrev == SCREEN_AUDIO_INPUT && scrPar.audio == AUDIO_PARAM_GAIN) {
                clearFlag = 0;
            }
            break;
        case SCREEN_AUDIO_INPUT:
            if (scrPrev == SCREEN_AUDIO_PARAM) {
                if (scrParPrev.audio == AUDIO_PARAM_GAIN) {
                    if (aProc->input != INPUT_TUNER) {
                        clearFlag = 0;
                    }
                }
            }
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
            if (scrPar.audio != scrParPrev.audio) {
                clearFlag = 1;
            }
            break;
        case SCREEN_AUDIO_INPUT:
            if (scrPar.input != scrParPrev.input) {
                clearFlag = 1;
            }
            break;
        default:
            break;
        }
    }

    // Save current screen and screen parameter
    scrPrev = screen;
    scrParPrev = scrPar;

    if (clearFlag) {
        screenClear();
    }
}

void screenReadSettings(void)
{
    uint16_t eeData;

    eeData = eeRead(EE_BRIGHTNESS_STBY);
    brStby = (eeData > LCD_BR_MAX ? 2 : (int8_t)(eeData));

    eeData = eeRead(EE_BRIGHTNESS_WORK);
    brWork = (eeData > LCD_BR_MAX ? LCD_BR_MAX : (int8_t)(eeData));
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
    glcdFill(canvas->color);
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
    AudioProc *aProc = audioProcGet();

    if (INPUT_TUNER == aProc->input)
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
    screenCheckChange();

    // Get new spectrum data
    if (swTimGetSpConvert() <= 0) {
        spGetADC(spData[SP_CHAN_LEFT].data, spData[SP_CHAN_RIGHT].data);

        improveSpectrum(&spData[SP_CHAN_LEFT]);
        improveSpectrum(&spData[SP_CHAN_RIGHT]);

        spReady = 1;
        swTimSetSpConvert(40);
    }

    switch (screen) {
    case SCREEN_STANDBY:
    case SCREEN_TIME:
        screenShowTime();
        break;
    case SCREEN_SPECTRUM:
        screenShowSpectrum();
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
    case SCREEN_SETUP:
        break;

    case SCREEN_TEST:
        screenTest();
        break;
    default:
        break;
    }

    screenUpdate();
}

void screenShowTime(void)
{
    RTC_type rtc;
    rtc.etm = rtcGetMode();
    const char **txtLabels = labelsGet();

    rtcGetTime(&rtc);

    if (canvas->showTime) {
        canvas->showTime(&rtc, (char *)txtLabels[LABEL_SUNDAY + rtc.wday]);
    }
}

void screenShowSpectrum(void)
{
    if (spReady) {
        if (canvas->showSpectrum) {
            canvas->showSpectrum(spData);
        }
    }
    spReady = 0;
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
    dp.icon = ICON24_BRIGHTNESS;

    if (canvas->showParam) {
        canvas->showParam(&dp);
    }
}

void screenShowInput(void)
{
    scrPar.audio = AUDIO_PARAM_GAIN;
    screenShowAudioParam();
}

void screenShowAudioParam(void)
{
    AudioProc *aProc = audioProcGet();
    AudioParam aPar = scrPar.audio;
    const char **txtLabels = labelsGet();

    if (aPar >= AUDIO_PARAM_END)
        aPar = AUDIO_PARAM_VOLUME;

    DispParam dp;
    if (aPar == AUDIO_PARAM_GAIN) {
        dp.label = txtLabels[LABEL_GAIN0 + aProc->input];
        dp.icon = ICON24_TUNER + aProc->input;
    } else {
        dp.label = txtLabels[LABEL_VOLUME + aPar];
        dp.icon = ICON24_VOLUME + aPar;
    }
    dp.value = aProc->item[aPar].value;
    dp.min = aProc->item[aPar].grid->min;
    dp.max = aProc->item[aPar].grid->max;
    dp.step = aProc->item[aPar].grid->step;

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

void screenTest(void)
{
    glcdSetFont(&fontterminus12);
    glcdSetFontColor(LCD_COLOR_WHITE);

    uint16_t *value;

    for (uint8_t i = 0; i < 16; i++) {
        glcdSetXY((i % 2) ? 28 : 0, 12 * (i / 2));
        value = (uint16_t *)(FLASH_BASE + EE_PAGE_SIZE * EE_PAGE_0 + 2 * i);
        glcdWriteNum(*value, 4, '0', 16);
    }

    for (uint8_t i = 0; i < 16; i++) {
        glcdSetXY((i % 2) ? 92 : 64, 12 * (i / 2));
        value = (uint16_t *)(FLASH_BASE + EE_PAGE_SIZE * EE_PAGE_1 + 2 * i);
        glcdWriteNum(*value, 4, '0', 16);
    }
}
