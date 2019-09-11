#ifndef LAYOUT_H
#define LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../display/glcd.h"
#include "icons.h"
#include "widget/spectrumcolumn.h"
#include "widget/stripedbar.h"
#include "widget/textedit.h"

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
    LayoutStripedBar bar;
} LayoutTune;

typedef struct {
    const tFont *stFont;        // Foft to draw station number
    const tFont *nameFont;      // Foft to draw station name
    LayoutStripedBar bar;
    uint8_t iconSpace;          // Space between icons
} LayoutTuner;

typedef struct {
    const tFont *psFont;        // Foft to draw PS
    const tFont *textFont;      // Foft to draw text
} LayoutRds;

typedef struct {
    GlcdRect rect;

    LayoutTime time;
    LayoutMenu menu;
    LayoutTune tune;
    LayoutTuner tuner;
    LayoutRds rds;
    LayoutTextEdit textEdit;

    const tFont *lblFont;       // Main label font
    const tFont *iconSet;       // Main icon set
} Layout;

const Layout *layoutGet(void);

// Modified fonts - full character set
extern const tFont fontterminusmod18x3;
extern const tFont fontterminusmod24x3;
// Modified fonts - for digits only
extern const tFont fontterminusdig30;
extern const tFont fontterminusdig40;
extern const tFont fontterminusdig58;
extern const tFont fontterminusdig64;
extern const tFont fontterminusdig80;
extern const tFont fontterminusdig96;
extern const tFont fontterminusdig120;

#ifdef __cplusplus
}
#endif

#endif // LAYOUT_H
