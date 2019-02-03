#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "../display/glcd.h"

typedef struct {
    const char *label;
    int16_t value;
    int16_t min;
    int16_t max;
    uint8_t mStep;
    uint8_t icon;
} DispParam;

typedef struct {
    int16_t sc;                 // Count of bar lines
    uint8_t sw;                 // Width of bar line
    int16_t barY;               // Y pos of the bar
    uint16_t barW;              // Width of the bar
    uint8_t half;               // Height of upper/lower bar part
    uint8_t middle;             // Height of middle bar part
} CanvasBar;

typedef struct {
    uint16_t fg;
    uint16_t bg;
    uint16_t spCol;
    uint16_t spPeak;
    uint16_t inactive;
    uint16_t active;
} CanvasPalette;

typedef struct {
    Glcd *glcd;
    const CanvasPalette *pal;
} Canvas;

void canvasInit(Canvas **value);
void canvasClear(void);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
