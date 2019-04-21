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

typedef struct {
    uint8_t fall[SPECTRUM_SIZE];
    uint8_t show[SPECTRUM_SIZE];
    uint8_t peak[SPECTRUM_SIZE];
    uint8_t old_show[SPECTRUM_SIZE];
} SpData;

typedef struct {
    GlcdRect rect;

    struct {
        const tFont *hmsFont;       // Font to draw hours/minutes/seconds
        const tFont *dmyFont;       // Font to draw day/month/year
        const tFont *wdFont;        // Font to draw weekday
        uint8_t hmsY;               // Y position of hours/minutes/seconds
        uint8_t dmyY;               // Y position of day/month/year
        uint8_t wdY;                // Y position of weekday
    } time;
    struct {
        const tFont *headFont;      // Font to draw menu header
        const tFont *menuFont;      // Foft to draw menu item
        uint8_t itemCnt;            // Number of items can be shown
    } menu;
    struct {
        const tFont *valFont;       // Foft to draw tune label
        int16_t valY;               // Y position of the tune value
        CanvasBar bar;
    } tune;
    struct {
        uint8_t step;               // Step in pixels between spectrum columns
        uint8_t oft;                // Offset of visible part of the column
        uint8_t width;              // Width of visible part of the column
        uint8_t wfH;                // Width of waterfile line
    } sp;
    struct {
        const tFont *stFont;        // Foft to draw station number
        const tFont *nameFont;      // Foft to draw station name
        CanvasBar bar;
        uint8_t iconSpace;          // Space between icons
    } tuner;
    struct {
        GlcdRect rect;
        const tFont *editFont;      // Editor font
    } textEdit;
    const tFont *lblFont;           // Main label font
    const tFont *iconSet;           // Main icon set
} Layout;

// Canvas variants
const Layout *lt160x128Get(void);
const Layout *lt176x132Get(void);
const Layout *lt220x176Get(void);
const Layout *lt320x240Get(void);
const Layout *lt400x240Get(void);
const Layout *lt480x320Get(void);
#ifdef EMUL_DISP
const Layout *ltEmulGet(void);
#endif

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
