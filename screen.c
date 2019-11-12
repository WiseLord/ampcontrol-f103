#include "screen.h"

#include "control.h"
#include "gui/canvas.h"
#include "menu.h"
#include "settings.h"
#include "swtimers.h"
#include "tr/labels.h"

static bool scrToClear = false;

static Screen screen = {
    .mode = SCREEN_STANDBY,
    .def = SCREEN_SPECTRUM,
};

static bool screenCheckClear(void)
{
    bool clear = false;

    static ScreenMode scrPrev = SCREEN_END;

    if (scrToClear) {
        clear = true;
        scrToClear = false;
    } else if (screen.mode != scrPrev) {
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
    }

    if (screen.mode != scrPrev || clear) {
        // Handle standby/work brightness
        screenSetBrightness((int8_t)settingsGet(screen.mode == SCREEN_STANDBY ?
                                                PARAM_DISPLAY_BR_STBY : PARAM_DISPLAY_BR_WORK));
    }


    // Save current screen and screen parameter
    scrPrev = screen.mode;

    return clear;
}

void screenReadSettings(void)
{
    screen.def = (ScreenMode)settingsRead(PARAM_DISPLAY_DEF);
}

void screenSaveSettings(void)
{
    settingsStore(PARAM_DISPLAY_DEF, screen.def);
}

void screenInit(void)
{
    labelsInit();
    canvasInit();

    screenReadSettings();
}

Screen *screenGet(void)
{
    return &screen;
}

void screenSetMode(ScreenMode value)
{
    screen.mode = value;
}

ScreenMode screenGetMode()
{
    return screen.mode;
}

void screenSetBrightness(int8_t value)
{
    screen.brightness = value;
}

void screenToClear(void)
{
    scrToClear = true;
}

void screenShow(bool clear)
{
    GlcdRect rect = canvasGet()->layout->rect;
    Spectrum *spectrum = spGet();
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    glcdSetRect(&rect);

    if (!clear) {
        clear = screenCheckClear();
        if (screen.mode == SCREEN_TEXTEDIT) {
            rect = canvasGet()->layout->textEdit.rect;
            if (clear) {
                const int16_t th = canvasGet()->glcd->drv->height / 100;
                glcdDrawFrame(rect.x - th, rect.y - th, rect.w + 2 * th, rect.h + 2 * th, th, canvasGet()->pal->fg);
            }
            glcdSetRect(&rect);
        }
    }

    if (screen.mode != SCREEN_STANDBY) {
        // Get new spectrum data
        if (swTimGet(SW_TIM_SP_CONVERT) <= 0) {
            swTimSet(SW_TIM_SP_CONVERT, 20);
            spGetADC(spectrum);
            spectrum->ready = true;
        }

        if (swTimGet(SW_TIM_INPUT_POLL) == 0) {
            if (inType == IN_TUNER) {
                tunerUpdateStatus();
                controlReportTuner(false);
            }
            swTimSet(SW_TIM_INPUT_POLL, 100);
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
        canvasShowTime(clear);
        break;
    case SCREEN_SPECTRUM:
        canvasShowSpectrum(clear);
        break;
    case SCREEN_AUDIO_PARAM:
        canvasShowTune(clear);
        break;
    case SCREEN_AUDIO_FLAG:
        canvasShowAudioFlag(clear);
        break;
    case SCREEN_AUDIO_INPUT:
        canvasShowAudioInput(clear, screen.iconHint);
        break;
    case SCREEN_MENU:
        canvasShowMenu(clear);
        break;
    case SCREEN_TEXTEDIT:
        canvasShowTextEdit(clear);
        break;
    case SCREEN_STBY_TIMER:
        canvasShowTimer(clear, swTimGet(SW_TIM_STBY_TIMER));
        break;
    case SCREEN_SILENCE_TIMER:
        canvasShowTimer(clear, swTimGet(SW_TIM_SILENCE_TIMER));
        break;
    default:
        break;
    }
}

void screenPwm(void)
{
    static int8_t br;

    if (++br >= LCD_BR_MAX) {
        br = 0;
    }

    if (br == screen.brightness) {
        pinsSetBckl(false);
    } else if (br == 0) {
        pinsSetBckl(true);
    }
}
