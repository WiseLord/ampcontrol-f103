#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "layout.h"
#include "palette.h"

typedef uint8_t ScreenType;
enum {
    // Screens allowed to be default
    SCREEN_SPECTRUM = 0,
    SCREEN_TIME,
    SCREEN_AUDIO_INPUT,

    SCREEN_STANDBY,

    SCREEN_AUDIO_PARAM,
    SCREEN_AUDIO_FLAG,

    SCREEN_MENU,

    SCREEN_TEXTEDIT,

    SCREEN_STBY_TIMER,
    SCREEN_SILENCE_TIMER,

    SCREEN_END
};

typedef struct {
    Glcd *glcd;
    const Palette *pal;
    const Layout *layout;
    TextEdit te;
} Canvas;

void canvasInit(void);
Canvas *canvasGet(void);

void canvasClear(void);

void canvasShowTime(bool clear);
void canvasShowMenu(bool clear);
void canvasShowTune(bool clear);
void canvasShowAudioFlag(bool clear);
void canvasShowSpectrum(bool clear);
void canvasShowTuner(bool clear);
void canvasShowKaradio(bool clear, Icon icon);
void canvasShowAudioInput(bool clear, Icon icon);
void canvasShowTextEdit(bool clear);
void canvasShowTimer(bool clear, int32_t value);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
