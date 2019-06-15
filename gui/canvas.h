#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "layout.h"

typedef struct {
    uint16_t fg;
    uint16_t bg;
    uint16_t gray;
    uint16_t spCol;
    uint16_t spPeak;
    uint16_t inactive;
    uint16_t active;
} Palette;

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
void canvasShowKaradio(bool clear);
void canvasShowAudioInput(bool clear, Icon icon);
void canvasShowTextEdit(bool clear);
void canvasShowTimer(bool clear, int32_t value);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
