#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "audio/audiodefs.h"
#include "layout.h"
#include "palette.h"

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
void canvasShowTune(bool clear, AudioTune tune);
void canvasShowAudioFlag(bool clear, AudioTune flag);
void canvasShowSpectrum(bool clear);
void canvasShowStars(bool clear, int16_t offset);
void canvasShowTuner(bool clear);
void canvasShowMpd(bool clear, Icon icon);
void canvasShowKaradio(bool clear, Icon icon);
void canvasShowAudioInput(bool clear, Icon icon);
void canvasShowTextEdit(bool clear);
void canvasShowTimer(bool clear, int32_t value);

void canvasDebugFPS(void);
void canvasDebugTimers(void);
void canvasDebugGpio(void);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
