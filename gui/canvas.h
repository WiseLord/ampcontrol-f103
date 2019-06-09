#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../display/glcd.h"
#include "widget/textedit.h"

typedef struct {
    uint16_t fg;
    uint16_t bg;
    uint16_t gray;
    uint16_t spCol;
    uint16_t spPeak;
    uint16_t inactive;
    uint16_t active;
} CanvasPalette;

typedef struct {
    Glcd *glcd;
    const CanvasPalette *pal;
    TextEdit te;
} Canvas;

void canvasInit(Canvas **value);
Canvas *canvasGet(void);

void canvasClear(void);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
