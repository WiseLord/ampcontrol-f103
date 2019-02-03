#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "../display/glcd.h"

#define TE_STR_LEN      64
#define TE_USTR_LEN     32

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
    char str[TE_STR_LEN];       // String to edit
    UChar uStr[TE_USTR_LEN];    // String being edited
    uint16_t uLen;              // Length of uStr
    int16_t sPos;               // Current symbol position
    int16_t lastPos;            // Last char position
} TextEdit;

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
    TextEdit te;
} Canvas;

void canvasInit(Canvas **value);
void canvasClear(void);

void texteditSet(char *text);
void texteditChange(int8_t value);
void texteditAddChar(void);
void texteditDelChar(void);
void texteditApply(void);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
