#ifndef LAYOUT_H
#define LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "canvas.h"

#include <stdint.h>
#include <stddef.h>

#include "../rtc.h"
#include "../spectrum.h"
#include "../audio/audiodefs.h"
#include "../tuner/stations.h"
#include "../tuner/tuner.h"

typedef union {
    struct {
        uint8_t show[SP_CHAN_END];   // Value to show
        uint8_t prev[SP_CHAN_END];   // Previous value
        uint8_t peak[SP_CHAN_END];   // Peak value
        uint8_t fall[SP_CHAN_END];   // Fall speed
    };
    struct {
        int16_t showW;
        int16_t prevW;
        int16_t peakW;
        int16_t fallW;
    };
} SpCol;

typedef struct {
    SpCol col[SPECTRUM_SIZE];
} SpDrawData;

typedef struct {
    const tFont *hmsFont;       // Font to draw hours/minutes/seconds
    const tFont *dmyFont;       // Font to draw day/month/year
    const tFont *wdFont;        // Font to draw weekday
    uint8_t hmsY;               // Y position of hours/minutes/seconds
    uint8_t dmyY;               // Y position of day/month/year
    uint8_t wdY;                // Y position of weekday
} LayoutTime;

typedef struct {
    const tFont *headFont;      // Font to draw menu header
    const tFont *menuFont;      // Foft to draw menu item
    uint8_t itemCnt;            // Number of items can be shown
} LayoutMenu;

typedef struct {
    const tFont *valFont;       // Foft to draw tune label
    int16_t valY;               // Y position of the tune value
    CanvasBar bar;
} LayoutTune;

typedef struct {
    uint8_t step;               // Step in pixels between spectrum columns
    uint8_t oft;                // Offset of visible part of the column
    uint8_t width;              // Width of visible part of the column
    uint8_t wfH;                // Width of waterfile line
} LayoutSpectrum;

typedef struct {
    const tFont *stFont;        // Foft to draw station number
    const tFont *nameFont;      // Foft to draw station name
    CanvasBar bar;
    uint8_t iconSpace;          // Space between icons
} LayoutTuner;

typedef struct {
    const tFont *psFont;        // Foft to draw PS
    const tFont *textFont;      // Foft to draw text
} LayoutRds;

typedef struct {
    GlcdRect rect;
    const tFont *editFont;      // Editor font
} LayoutTextEdit;

typedef struct {
    GlcdRect rect;

    LayoutTime time;
    LayoutMenu menu;
    LayoutTune tune;
    LayoutSpectrum sp;
    LayoutTuner tuner;
    LayoutRds rds;
    LayoutTextEdit textEdit;

    const tFont *lblFont;       // Main label font
    const tFont *iconSet;       // Main icon set
} Layout;

// Canvas variants
void layoutInit(void);
const Layout *layoutGet(void);

void layoutShowTime(bool clear);
void layoutShowMenu(bool clear);
void layoutShowTune(bool clear, AudioTune tune);
void layoutShowSpectrum(bool clear);
void layoutShowTuner(bool clear);
void layoutShowTextEdit(bool clear);
void layoutShowTimer(bool clear, int32_t value);

#ifdef __cplusplus
}
#endif

#endif // LAYOUT_H
