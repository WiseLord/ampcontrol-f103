#include "screen.h"

#include "amp.h"
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

    static ScrMode scrPrev = SCREEN_END;

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
    screen.def = (ScrMode)settingsGet(PARAM_DISPLAY_DEF);
}

void screenSaveSettings(void)
{
    settingsSet(PARAM_DISPLAY_DEF, screen.def);
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

void screenSetMode(ScrMode value)
{
    screen.mode = value;
}

void screenChangeDef(void)
{
    if (++screen.def >= SCREEN_STANDBY) {
        screen.def = SCREEN_SPECTRUM;
    }
}

void screenSetBrightness(int8_t value)
{
    screen.brightness = value;
}

void screenToClear(void)
{
    scrToClear = true;
}

void screenShow(void)
{
    GlcdRect rect = canvasGet()->layout->rect;
    Spectrum *spectrum = spGet();
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    glcdSetRect(&rect);

    bool clear = screenCheckClear();
    if (screen.mode == SCREEN_TEXTEDIT) {
        rect = canvasGet()->layout->textEdit.rect;
        if (clear) {
            const int16_t th = canvasGet()->glcd->drv->height / 100;
            glcdDrawFrame(rect.x - th, rect.y - th, rect.w + 2 * th, rect.h + 2 * th, th, canvasGet()->pal->fg);
        }
        glcdSetRect(&rect);
    }

    if (screen.mode != SCREEN_STANDBY) {
        // Get new spectrum data
        if (swTimGet(SW_TIM_SP_CONVERT) == 0) {
            swTimSet(SW_TIM_SP_CONVERT, 20);

            spGetADC(SP_CHAN_LEFT, spectrum->data[SP_CHAN_LEFT].raw, SPECTRUM_SIZE);
            spGetADC(SP_CHAN_RIGHT, spectrum->data[SP_CHAN_RIGHT].raw, SPECTRUM_SIZE);
            spectrum->ready = true;
        }

        if (swTimGet(SW_TIM_INPUT_POLL) == 0) {
            if (inType == IN_TUNER) {
                tunerUpdateStatus();
                controlReportTunerFreq(false);
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
