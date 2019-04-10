#include "screen.h"

#include "canvas/layout.h"
#include "display/glcd.h"
#include "tr/labels.h"
#include "actions.h"
#include "eemap.h"
#include "fft.h"
#include "spectrum.h"
#include "swtimers.h"

static Screen screen = SCREEN_STANDBY;
static Screen screenDefault = SCREEN_SPECTRUM;
static ScreenParam scrPar;

// TODO: Read from backup memory
static int8_t brightness[BR_END];

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
                    InputType inType = aProc->par.inType[aProc->par.input];
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
            swTimSet(SW_TIM_TUNER_POLL, 100);
        } else {
            swTimSet(SW_TIM_TUNER_POLL, SW_TIM_OFF);
        }

        // Handle standby/work brightness
        if (screen == SCREEN_STANDBY) {
            screenChangeBrighness(BR_STBY, 0);
        } else {
            screenChangeBrighness(BR_WORK, 0);
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
    for (BrMode mode = BR_STBY; mode < BR_END; mode++) {
        brightness[mode] = (int8_t)eeRead(EE_DISPLAY_BR_STBY + mode);
        if (brightness[mode] < LCD_BR_MIN) {
            brightness[mode] = LCD_BR_MIN;
        } else if (brightness[mode] > LCD_BR_MAX) {
            brightness[mode] = LCD_BR_MAX;
        }
    }
}

void screenSaveSettings(void)
{
    eeUpdate(EE_DISPLAY_BR_WORK, brightness[BR_WORK]);
}


void screenInit(void)
{
    labelsInit();
    layoutInit();
    canvasClear();
    screenReadSettings();
    dispdrvSetBrightness(brightness[BR_STBY]);
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

    InputType inType = aProc->par.inType[aProc->par.input];;

    int32_t stbyTimer = swTimGet(SW_TIM_STBY_TIMER);

    if (stbyTimer > 0 && stbyTimer < 60 * 1000 + 999) {
        return  SCREEN_STBY_TIMER;
    }

    if (IN_TUNER == inType)
        return SCREEN_TUNER;

    return screenDefault;
}


int8_t screenGetBrightness(BrMode mode)
{
    return brightness[mode];
}

void screenSetBrightness(BrMode mode, int8_t value)
{
    brightness[mode] = value;

    dispdrvSetBrightness(value);
}

void screenChangeBrighness(BrMode mode, int8_t diff)
{
    int8_t br = screenGetBrightness(mode);

    br += diff;

    if (br > LCD_BR_MAX)
        br = LCD_BR_MAX;
    if (br < LCD_BR_MIN)
        br = LCD_BR_MIN;

    screenSetBrightness(mode, br);
}

void screenShow(bool clear)
{
    Spectrum *spectrum = spGet();

    GlcdRect rect = layoutGet()->rect;

    if (!clear) {
        clear = screenCheckClear();
        if (screen == SCREEN_TEXTEDIT) {
            rect = layoutGet()->textEdit.rect;
        }
    }
    glcdSetRect(rect);

    // Get new spectrum data
    if (swTimGet(SW_TIM_SP_CONVERT) <= 0) {
        swTimSet(SW_TIM_SP_CONVERT, 20);
        spGetADC(spectrum->chan[SP_CHAN_LEFT].raw, spectrum->chan[SP_CHAN_RIGHT].raw);
        spectrum->ready = true;
    }

    if (clear) {
        spectrum->wtfX = 0;
        canvasClear();
        spectrum->redraw = true;
    }

    switch (screen) {
    case SCREEN_STANDBY:
    case SCREEN_TIME:
        layoutShowTime(clear);
        break;
    case SCREEN_SPECTRUM:
        layoutShowSpectrum(clear);
        break;
    case SCREEN_AUDIO_INPUT:
        layoutShowTune(clear, AUDIO_TUNE_GAIN);
        break;
    case SCREEN_AUDIO_PARAM:
        layoutShowTune(clear, scrPar.tune);
        break;
    case SCREEN_TUNER:
        if (swTimGet(SW_TIM_TUNER_POLL) == 0) {
            tunerUpdateStatus();
            swTimSet(SW_TIM_TUNER_POLL, 100);
        }
        layoutShowTuner(clear);
        break;
    case SCREEN_MENU:
        layoutShowMenu(clear);
        break;
    case SCREEN_TEXTEDIT:
        layoutShowTextEdit(clear);
        break;
    case SCREEN_STBY_TIMER:
        layoutShowTimer(clear, swTimGet(SW_TIM_STBY_TIMER));
        break;
    default:
        break;
    }
}
