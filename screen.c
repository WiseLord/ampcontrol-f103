#include "screen.h"

#include "actions.h"
#include "display/glcd.h"
#include "fft.h"
#include "gui/layout.h"
#include "spectrum.h"
#include "swtimers.h"
#include "tr/labels.h"

static ScreenParam scrPar;
static bool scrToClear = false;

static Screen screen = {
    .mode = SCREEN_STANDBY,
    .def = SCREEN_SPECTRUM,
};

static bool screenCheckClear(void)
{
    bool clear = false;

    static ScreenMode scrPrev = SCREEN_STANDBY;
    static ScreenParam scrParPrev;

    // Check if we need to clear screen
    if (screen.mode != scrPrev) {
        clear = true;
        switch (screen.mode) {
        case SCREEN_STANDBY:
        case SCREEN_TIME:
            if (scrPrev == SCREEN_STANDBY || scrPrev == SCREEN_TIME) {
                clear = false;
            }
            break;
        case SCREEN_MENU:
            menuGet()->selected = 0;
            break;
        default:
            break;
        }

        // Enable/disable tuner polling
        if (screen.mode == SCREEN_TUNER) {
            swTimSet(SW_TIM_TUNER_POLL, 100);
        } else {
            swTimSet(SW_TIM_TUNER_POLL, SW_TIM_OFF);
        }

        // Handle standby/work brightness
        if (screen.mode == SCREEN_STANDBY) {
            screenChangeBrighness(BR_STBY, 0);
        } else {
            screenChangeBrighness(BR_WORK, 0);
        }
    } else {
        switch (screen.mode) {
        case SCREEN_SPECTRUM:
            if (scrPar.spMode != scrParPrev.spMode) {
                clear = true;
            }
            break;
        case SCREEN_AUDIO_PARAM:
            if (scrPar.tune != scrParPrev.tune) {
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
    scrPrev = screen.mode;
    scrParPrev = scrPar;

    return clear;
}

void screenReadSettings(void)
{
    for (BrMode brMode = BR_STBY; brMode < BR_END; brMode++) {
        screen.br[brMode] = (int8_t)settingsRead((Param)(PARAM_DISPLAY_BR_STBY + brMode));
        if (screen.br[brMode] < LCD_BR_MIN) {
            screen.br[brMode] = LCD_BR_MIN;
        } else if (screen.br[brMode] > LCD_BR_MAX) {
            screen.br[brMode] = LCD_BR_MAX;
        }
    }
}

void screenSaveSettings(void)
{
    settingsStore(PARAM_DISPLAY_BR_WORK, screen.br[BR_WORK]);
}


void screenInit(void)
{
    labelsInit();
    layoutInit();
    canvasClear();
    screenReadSettings();
    dispdrvSetBrightness(screen.br[BR_STBY]);
}

void screenSet(ScreenMode value)
{
    screen.mode = value;
}

ScreenMode screenGet()
{
    return screen.mode;
}


void screenSetParam(ScreenParam param)
{
    scrPar = param;
}


void screenSetDefault(ScreenMode value)
{
    screen.def = value;
}

ScreenMode screenGetDefault(void)
{
    AudioProc *aProc = audioGet();

    InputType inType = aProc->par.inType[aProc->par.input];;

    int32_t timer;

    timer = swTimGet(SW_TIM_STBY_TIMER);
    if (timer > 0 && timer < 60 * 1000 + 999) {
        return SCREEN_STBY_TIMER;
    }

    timer = swTimGet(SW_TIM_SILENCE_TIMER);
    if (timer > 0 && timer < 30 * 1000 + 999) {
        return SCREEN_SILENCE_TIMER;
    }

    if (IN_TUNER == inType)
        return SCREEN_TUNER;

    return screen.def;
}


int8_t screenGetBrightness(BrMode mode)
{
    return screen.br[mode];
}

void screenSetBrightness(BrMode mode, int8_t value)
{
    screen.br[mode] = value;

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

void screenToClear(void)
{
    scrToClear = true;
}

void screenShow(bool clear)
{
    Spectrum *spectrum = spGet();

    GlcdRect rect = layoutGet()->rect;

    if (scrToClear) {
        clear = true;
        scrToClear = false;
    }

    if (!clear) {
        clear = screenCheckClear();
        if (screen.mode == SCREEN_TEXTEDIT) {
            rect = layoutGet()->textEdit.rect;
        }
    }
    glcdSetRect(rect);

    if (screen.mode != SCREEN_STANDBY) {
        // Get new spectrum data
        if (swTimGet(SW_TIM_SP_CONVERT) <= 0) {
            swTimSet(SW_TIM_SP_CONVERT, 20);
            spGetADC(spectrum);
            spectrum->ready = true;
        }
    }

    if (clear) {
        canvasClear();
        spectrum->wtfX = 0;
        spectrum->redraw = true;
    }

    switch (screen.mode) {
    case SCREEN_STANDBY:
    case SCREEN_TIME:
        layoutShowTime(clear);
        break;
    case SCREEN_SPECTRUM:
        layoutShowSpectrum(clear);
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
    case SCREEN_SILENCE_TIMER:
        layoutShowTimer(clear, swTimGet(SW_TIM_SILENCE_TIMER));
        break;
    default:
        break;
    }
}
