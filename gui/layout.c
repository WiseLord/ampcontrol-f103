#include "layout.h"

#include "../menu.h"
#include "../tr/labels.h"
#include "../tuner/rds.h"

#include <string.h>

static const Layout *lt;
static Canvas *canvas;

static SpDrawData spDrawData;

static uint16_t level2color(int16_t value);
static void drawBar(const CanvasBar *bar, int16_t value, int16_t min, int16_t max);
static void drawTm(RTC_type *rtc, RtcMode tm);
static void drawMenuItem(uint8_t idx, const tFont *fontItem);
static uint8_t calcSpCol(Spectrum *sp, int16_t chan, int16_t scale, uint8_t col, SpCol *spCol);
static void drawSpCol(bool redraw, int16_t x, int16_t y, int16_t w, int16_t h, SpCol *col);
static void drawWaterfall(Spectrum *sp);
static void drawSpectrum(Spectrum *sp, SpChan chan, GlcdRect *rect);
static void drawRds(Rds *rds);


static uint16_t level2color(int16_t value)
{
    uint16_t color = 0xFFFF;

    if (value < 32) {           // Black => Blue
        color = 0x0000;
        color |= (value - 0);
    } else if (value < 64) {    // Blue => Cyan
        color = 0x003F;
        color |= ((value - 32) << 6);
    } else if (value < 96) {    // Cyan => Olive
        color = 0x07E0;
        color |= (95 - value);
    } else if (value < 128) {   // Olive => Yellow
        color = 0x07E0;
        color |= ((value - 96) << 11);
    } else if (value < 160) {   // Yellow => Red
        color = 0xF800;
        color |= ((159 - value) << 6);
    } else if (value < 192) {   // Red => Purple
        color = 0xF800;
        color |= (value - 160);
    } else if (value < 224) {   // Purple => White
        color = 0xF83F;
        color |= ((value - 160) << 6);
    }

    return color;
}

static void drawBar(const CanvasBar *bar, int16_t value, int16_t min, int16_t max)
{
    const int16_t sc = bar->sc;         // Scale count
    const uint8_t sw = bar->sw;         // Scale width
    const int16_t barPos = bar->barY;
    const uint8_t barHalf = bar->half;
    const uint8_t barMiddle = bar->middle;
    const int16_t width = bar->barW;

    if (min + max) { // Non-symmectic scale => rescale to 0..sc
        value = sc * (value - min) / (max - min);
    } else { // Symmetric scale => rescale to -sc/2..sc/2
        value = (sc / 2) * value / (max ? max : 1);
    }

    for (uint16_t i = 0; i < sc; i++) {
        uint16_t color = canvas->pal->fg;

        if (min + max) { // Non-symmetric scale
            if (i >= value) {
                color = canvas->pal->bg;
            }
        } else { // Symmetric scale
            if ((value > 0 && i >= value + (sc / 2)) ||
                (value >= 0 && i < (sc / 2 - 1)) ||
                (value < 0 && i < value + (sc / 2)) ||
                (value <= 0 && i > (sc / 2))) {
                color = canvas->pal->bg;
            }
        }

        glcdDrawRect(i * (width / sc), barPos, sw, barHalf, color);
        glcdDrawRect(i * (width / sc), barPos + barHalf, sw, barMiddle, canvas->pal->fg);
        glcdDrawRect(i * (width / sc), barPos + barHalf + barMiddle, sw, barHalf, color);
    }
}

static void drawTm(RTC_type *rtc, RtcMode tm)
{
    int8_t time = *((int8_t *)rtc + tm);

    glcdSetFontColor(canvas->pal->fg);
    glcdSetFontBgColor(canvas->pal->bg);
    if (rtc->etm == tm) {
        glcdSetFontColor(canvas->pal->bg);
        glcdSetFontBgColor(canvas->pal->fg);
    }
    glcdWriteUChar(LETTER_SPACE_CHAR);
    if (tm == RTC_YEAR) {
        glcdWriteString("20");
        glcdWriteUChar(LETTER_SPACE_CHAR);
    }
    if (time >= 0) {
        glcdWriteNum(time, 2, '0', 10);
    } else {
        glcdWriteString("--");
    }
    glcdWriteUChar(LETTER_SPACE_CHAR);
    glcdSetFontColor(canvas->pal->fg);
    glcdSetFontBgColor(canvas->pal->bg);
}

static void drawMenuItem(uint8_t idx, const tFont *fontItem)
{
    uint8_t fIh = (uint8_t)fontItem->chars[0].image->height;

    Menu *menu = menuGet();
    uint8_t items = menu->dispSize;

    int16_t width = lt->rect.w;
    MenuIdx menuIdx = menu->list[idx];
    const char *name = menuGetName(menuIdx);
    uint8_t active = (menu->active == menu->list[idx]);

    const uint8_t ih = fIh + 4; // Menu item height
    int16_t y_pos = lt->rect.h - ih * (items - idx + menu->dispOft);

    // Draw selection frame
    glcdDrawFrame(0, y_pos, width, ih, 1, active ? canvas->pal->fg : canvas->pal->bg);

    // Draw menu name
    glcdSetFont(fontItem);
    glcdSetFontColor(canvas->pal->fg);

    glcdSetXY(4, y_pos + 2);
    if (menu->list[idx] != MENU_NULL) {
        glcdWriteString("  ");
    } else {
        glcdWriteString("< ");
    }
    glcdWriteStringConst(name);

    // Draw menu value
    int16_t x = canvas->glcd->x;
    glcdSetXY(width - 2, y_pos + 2);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);

    // Inverse value color if selected
    uint16_t color = canvas->glcd->font.color;
    uint16_t bgColor = canvas->glcd->font.bgColor;
    if (active && menu->selected) {
        glcdSetFontColor(bgColor);
        glcdSetFontBgColor(color);
    }
    glcdSetStringFramed(true);
    uint16_t strLen = glcdWriteStringConst(menuGetValueStr(menuIdx));
    glcdSetStringFramed(false);

    glcdSetFontColor(color);
    glcdSetFontBgColor(bgColor);

    // Fill space between name and value
    glcdDrawRect(x, y_pos + 2, width - 2 - x - strLen, fIh, canvas->pal->bg);
}

static uint8_t calcSpCol(Spectrum *sp, int16_t chan, int16_t scale, uint8_t col, SpCol *spCol)
{
    int16_t raw;

    SpCol *spDrawCol = &spDrawData.col[col];

    if (chan == SP_CHAN_BOTH) {
        uint8_t rawL = sp->data[SP_CHAN_LEFT].raw[col];
        uint8_t rawR = sp->data[SP_CHAN_RIGHT].raw[col];
        if (rawL > rawR) {
            raw = rawL;
        } else {
            raw = rawR;
        }
        *spCol = *spDrawCol;
    } else {
        raw = sp->data[chan].raw[col];
        spCol->showW = spDrawCol->show[chan];
        spCol->prevW = spDrawCol->prev[chan];
        spCol->peakW = spDrawCol->peak[chan];
        spCol->fallW = spDrawCol->fall[chan];
    }

    raw = scale * raw / N_DB;

    spCol->prevW = spCol->showW;
    if (raw < spCol->showW) {
        if (spCol->showW >= spCol->fallW) {
            spCol->showW -= spCol->fallW;
            spCol->fallW++;
        } else {
            spCol->showW = 0;
        }
    }

    if (raw > spCol->showW) {
        spCol->showW = raw;
        spCol->fallW = 1;
    }

    if (spCol->peakW <= raw) {
        spCol->peakW = raw + 1;
    } else {
        if (spCol->peakW && spCol->peakW > spCol->showW + 1) {
            spCol->peakW--;
        }
    }

    if (chan == SP_CHAN_BOTH) {
        *spDrawCol = *spCol;
    } else {
        spDrawCol->show[chan] = (uint8_t)spCol->showW;
        spDrawCol->prev[chan] = (uint8_t)spCol->prevW;
        spDrawCol->peak[chan] = (uint8_t)spCol->peakW;
        spDrawCol->fall[chan] = (uint8_t)spCol->fallW;
    }

    return (uint8_t)(spCol->showW * N_DB / scale);
}

static void drawSpCol(bool redraw, int16_t x, int16_t y, int16_t w, int16_t h, SpCol *col)
{
    int16_t s = col->showW;
    int16_t os = col->prevW;
    int16_t p = col->peakW;

    const CanvasPalette *pal = canvas->pal;
    if (s == 0) {
        s = 1;
    }
    if (s >= h) {
        s = h - 1;
    }
    if (p >= h) {
        p = h - 1;
    }
    if (os >= h) {
        os = h - 1;
    }

    // Full redraw the column
    if (redraw) {
        glcdDrawRect(x, y + h - s, w, s, pal->spCol);

        if (p > s) {
            glcdDrawRect(x, y + h - p, w, 1, pal->spPeak);
        }
        return;
    }

    // Draw part of changed column
    if (s > os) {
        glcdDrawRect(x, y + h - s, w, s - os, pal->spCol);
    } else if (s < os) {
        glcdDrawRect(x, y + h - os, w, os - s, pal->bg);
    }

    // Clear old peak
    if (p >= s) {
        glcdDrawRect(x, y + h - p - 1, w, 1, pal->bg);
    }

    // Draw new peak
    if (p > s) {
        glcdDrawRect(x, y + h - p, w, 1, pal->spPeak);
    }
}

static void drawWaterfall(Spectrum *sp)
{
    if (++sp->wtfX >= lt->rect.w) {
        sp->wtfX = 0;
    }

    const uint8_t wfH = lt->sp.wfH;

    glcdShift((sp->wtfX + 1) % lt->rect.w);

    for (uint8_t col = 0; col < (lt->rect.h + wfH - 1) / wfH; col++) {

        SpCol spCol;
        uint8_t raw = calcSpCol(sp, SP_CHAN_BOTH, lt->rect.h, col, &spCol);

        uint16_t color = level2color(raw);
        glcdDrawRect(sp->wtfX, lt->rect.h - 1 - (col * wfH), 1, wfH, color);
    }
}

static void drawSpectrum(Spectrum *sp, SpChan chan, GlcdRect *rect)
{
    const uint8_t step = lt->sp.step;
    const uint8_t oft = lt->sp.oft;
    const int16_t width = lt->sp.width;

    const int16_t height = rect->h;
    const int16_t y = rect->y;

    const int16_t num = (lt->rect.w + width - 1) / step;    // Number of spectrum columns

    if (sp->redraw) {
        memset(&spDrawData, 0, sizeof (SpDrawData));
        memset(sp->data, 0, sizeof (SpData) * SP_CHAN_END);
    }

    for (uint8_t col = 0; col < num; col++) {
        int16_t x = oft + col * step;

        SpCol spCol;
        calcSpCol(sp, chan, height, col, &spCol);
        if (!sp->peaks) {
            spCol.peakW = 0;
        }
        drawSpCol(sp->redraw, x, y, width, height, &spCol);
    }
}

static void drawRds(Rds *rds)
{
    glcdSetFont(lt->rds.psFont);
    glcdSetXY(0, lt->rect.h / 2);
    uint16_t nameLen = glcdWriteString(rds->PS);

    glcdDrawRect(canvas->glcd->x, canvas->glcd->y,
                 lt->tuner.bar.barW - nameLen, lt->tuner.nameFont->chars[0].image->height,
                 canvas->pal->bg);


    glcdSetFont(lt->rds.textFont);
    glcdSetXY(0, lt->rect.h / 2 + lt->tuner.nameFont->chars[0].image->height);
    glcdWriteString(rds->text);
}

void layoutInit()
{
    lt = layoutGet();

    canvasInit(&canvas);

    bool rotate = settingsRead(PARAM_DISPLAY_ROTATE);
    glcdRotate(rotate);

    canvas->glcd->rect = lt->rect;

    menuGet()->dispSize = lt->menu.itemCnt;
}

void layoutShowTime(bool clear)
{
    (void)clear;

    RTC_type rtcStruct; // TODO: Use one struct in rtc driver
    rtcStruct.etm = rtcGetMode();

    RTC_type *rtc = &rtcStruct;

    rtcGetTime(rtc);

    int16_t zeroPos;
    int16_t ltspPos;
    int16_t timeLen;

    // HH:MM:SS
    glcdSetFont(lt->time.hmsFont);
    zeroPos = glcdFontSymbolPos('0');
    ltspPos = glcdFontSymbolPos(LETTER_SPACE_CHAR);
    timeLen = 6 * (lt->time.hmsFont->chars[zeroPos].image->width);    // 6 digits HHMMSS
    timeLen += 15 * (lt->time.hmsFont->chars[ltspPos].image->width);  // 13 letter spaces + 2 ':'
    glcdSetXY((lt->rect.w - timeLen) / 2, lt->time.hmsY);

    drawTm(rtc, RTC_HOUR);
    glcdWriteUChar(LETTER_SPACE_CHAR);
    glcdWriteUChar(':');
    glcdWriteUChar(LETTER_SPACE_CHAR);
    drawTm(rtc, RTC_MIN);
    glcdWriteUChar(LETTER_SPACE_CHAR);
    glcdWriteUChar(':');
    glcdWriteUChar(LETTER_SPACE_CHAR);
    drawTm(rtc, RTC_SEC);

    // DD:MM:YYYY
    glcdSetFont(lt->time.dmyFont);
    zeroPos = glcdFontSymbolPos('0');
    ltspPos = glcdFontSymbolPos(LETTER_SPACE_CHAR);
    timeLen = 8 * (lt->time.dmyFont->chars[zeroPos].image->width);    // 8 digits HHMMSS
    timeLen += 17 *
               (lt->time.dmyFont->chars[ltspPos].image->width);  // 15 letter spaces + 2 '.'
    glcdSetXY((lt->rect.w - timeLen) / 2, lt->time.dmyY);

    drawTm(rtc, RTC_DATE);
    glcdWriteUChar(LETTER_SPACE_CHAR);
    glcdWriteUChar('.');
    glcdWriteUChar(LETTER_SPACE_CHAR);
    drawTm(rtc, RTC_MONTH);
    glcdWriteUChar(LETTER_SPACE_CHAR);
    glcdWriteUChar('.');
    glcdWriteUChar(LETTER_SPACE_CHAR);
    drawTm(rtc, RTC_YEAR);

    // Weekday
    glcdSetFont(lt->time.wdFont);
    glcdSetFontColor(canvas->pal->fg);

    static int8_t wdayOld = 0;
    int8_t wday = rtc->wday;
    if (wday != wdayOld)    // Clear the area with weekday label
        glcdDrawRect(0, lt->time.wdY, lt->rect.w,
                     (int16_t)lt->time.wdFont->chars[0].image->height, canvas->pal->bg);
    wdayOld = wday;

    const char *wdayLabel = labelsGet((Label)(LABEL_SUNDAY + wday));

    glcdSetXY(lt->rect.w / 2, lt->time.wdY);
    glcdSetFontAlign(FONT_ALIGN_CENTER);
    glcdWriteStringConst(wdayLabel);
}

void layoutShowMenu(bool clear)
{
    (void)clear;

    Menu *menu = menuGet();

    const int16_t fHh = (int16_t)lt->menu.headFont->chars[0].image->height;
    const int16_t fIh = (int16_t)lt->menu.menuFont->chars[0].image->height;
    const uint8_t items = menu->dispSize;

    const int16_t dividerPos = (lt->rect.h - (fIh + 4) * items + fHh) / 2;

    // Show header
    const char *parentName = menuGetName(menu->parent);
    glcdSetFont(lt->menu.headFont);
    glcdSetFontColor(canvas->pal->fg);

    glcdSetXY(2, 0);
    glcdWriteStringConst(parentName);
    // Fill free space after header
    glcdDrawRect(canvas->glcd->x, canvas->glcd->y, lt->rect.w - canvas->glcd->x, fHh, canvas->pal->bg);

    glcdDrawRect(0, dividerPos, lt->rect.w, 1, canvas->glcd->font.color);

    for (uint8_t idx = 0; idx < menu->listSize; idx++) {
        if (idx >= menu->dispOft && idx < items + menu->dispOft) {
            drawMenuItem(idx, lt->menu.menuFont);
        }
    }
}

void layoutShowTune(bool clear)
{
    const tFont *iconSet = lt->iconSet;
    static int16_t valueOld;

    Spectrum *sp = spGet();
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    const char *label = labelsGet(LABEL_IN_TUNER + inType);
    Icon icon = (ICON_TUNER + inType);

    if (aProc->tune < AUDIO_TUNE_GAIN) {
        label = labelsGet(LABEL_VOLUME + aProc->tune);
        icon = ICON_VOLUME + aProc->tune;
    }

    const int16_t value = aProc->par.item[aProc->tune].value;

    const AudioGrid *grid = aProc->par.item[aProc->tune].grid;
    const int8_t min = grid ? grid->min : 0;
    const int8_t max = grid ? grid->max : 0;
    const uint8_t mStep = grid ? grid->mStep : 0;


    if (clear) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvas->pal->fg);
        glcdSetXY(0, 0);
        glcdWriteStringConst(label);
        // Icon
        glcdSetXY(lt->rect.w - iconSet->chars[0].image->width, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas->pal->fg, canvas->pal->bg);
    }

    if (clear || valueOld != value) {
        // Bar
        drawBar(&lt->tune.bar, value, min, max);
        // Value
        glcdSetXY(lt->rect.w, lt->tune.valY);
        glcdSetFontAlign(FONT_ALIGN_RIGHT);
        glcdSetFont(lt->tune.valFont);
        glcdWriteNum((value * mStep) / 8, 3, ' ', 10);
    }
    valueOld = value;

    // Spectrum
    if (!sp->ready) {
        return;
    }

    GlcdRect rect = canvas->glcd->rect;
    rect.h /= 2;
    rect.y = rect.h;
    drawSpectrum(sp, SP_CHAN_BOTH, &rect);

    sp->redraw = false;
    sp->ready = false;
}

void layoutShowAudioFlag(bool clear)
{
    AudioProc *aProc = audioGet();
    const tFont *iconSet = lt->iconSet;
    Label label;
    Icon icon;

    switch (aProc->tune) {
    case AUDIO_FLAG_LOUDNESS:
        label = (Label)(LABEL_MENU + MENU_RC_LOUDNESS);
        icon = aProc->par.loudness ? ICON_LOUDNESS_ON : ICON_LOUDNESS_OFF;
        break;
    case AUDIO_FLAG_SURROUND:
        label = (Label)(LABEL_MENU + MENU_RC_SURROUND);
        icon = aProc->par.surround ? ICON_SURROUND_ON : ICON_SURROUND_OFF;
        break;
    case AUDIO_FLAG_EFFECT3D:
        label = (Label)(LABEL_MENU + MENU_RC_EFFECT_3D);
        icon = aProc->par.effect3d ? ICON_EFFECT_3D_ON : ICON_EFFECT_3D_OFF;
        break;
    case AUDIO_FLAG_BYPASS:
        label = (Label)(LABEL_MENU + MENU_RC_TONE_BYPASS);
        icon = aProc->par.bypass ? ICON_TONE_BYPASS_ON : ICON_TONE_BYPASS_OFF;
        break;
    default:
        label = (Label)(LABEL_MENU + MENU_RC_MUTE);
        icon = aProc->par.mute ? ICON_MUTE_ON : ICON_MUTE_OFF;
        break;
    }

    if (clear) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvas->pal->fg);
        glcdSetXY(0, 0);
        glcdWriteStringConst(labelsGet(label));
    }

    // Icon
    static Icon iconOld = ICON_MUTE_OFF;
    if (clear || icon != iconOld) {
        glcdSetXY(lt->rect.w - iconSet->chars[0].image->width, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas->pal->fg, canvas->pal->bg);
        iconOld = icon;
    }

    // Spectrum
    Spectrum *sp = spGet();

    if (!sp->ready) {
        return;
    }

    GlcdRect rect = canvas->glcd->rect;
    rect.h /= 2;
    rect.y = rect.h;
    drawSpectrum(sp, SP_CHAN_BOTH, &rect);

    sp->redraw = false;
    sp->ready = false;
}

void layoutShowSpectrum(bool clear)
{
    (void)clear;

    Spectrum *sp = spGet();

    if (!sp->ready) {
        return;
    }

    GlcdRect rect = canvas->glcd->rect;

    switch (sp->mode) {
    case SP_MODE_STEREO:
        rect.h /= 2;
        drawSpectrum(sp, SP_CHAN_LEFT, &rect);
        rect.y = rect.h;
        drawSpectrum(sp, SP_CHAN_RIGHT, &rect);
        break;
    case SP_MODE_MIXED:
        drawSpectrum(sp, SP_CHAN_BOTH, &rect);
        break;
    case SP_MODE_WATERFALL:
        drawWaterfall(sp);
        break;
    default:
        break;
    }

    sp->redraw = false;
    sp->ready = false;
}

void layoutShowTuner(bool clear)
{
    Tuner *tuner = tunerGet();
    Spectrum *sp = spGet();
    Rds *rds = rdsGet();

    const tFont *iconSet = lt->iconSet;

    const tImage *icon = NULL;;
    const uint8_t iconSpace = lt->tuner.iconSpace;
    const CanvasBar *bar = &lt->tuner.bar;

    // Frequency
    uint16_t freq = tuner->status.freq;
    static uint16_t freqOld = 0;

    if (clear || freqOld != freq) {
        int16_t freqMin = (int16_t)tuner->par.fMin;
        int16_t freqMax = (int16_t)tuner->par.fMax;

        const tFont *fmFont = lt->lblFont;

        glcdSetFont(fmFont);
        glcdSetFontColor(canvas->pal->fg);
        glcdSetXY(0, 0);

        glcdWriteString("FM ");

        glcdWriteNum(freq / 100, 3, ' ', 10);
        glcdWriteUChar(LETTER_SPACE_CHAR);
        glcdWriteUChar('.');
        glcdWriteUChar(LETTER_SPACE_CHAR);
        glcdWriteNum(freq % 100, 2, '0', 10);

        // Scale
        drawBar(bar, (int16_t)freq, freqMin, freqMax);

        glcdSetFont(lt->tuner.stFont);
        glcdSetXY(lt->rect.w, lt->tune.valY);
        glcdSetFontAlign(FONT_ALIGN_RIGHT);

        // Station number
        int8_t stNum = stationGetNum(freq);
        if (stNum >= 0) {
            glcdWriteNum(stNum + 1, 2, ' ', 10);
        } else {
            glcdWriteString("--");
        }

        // Station name
        glcdSetFont(lt->tuner.nameFont);

        int16_t fh = glcdGetFontHeight(lt->tuner.nameFont);
        glcdSetXY(0, lt->rect.h / 2 - fh);

        uint16_t nameLen = glcdWriteString(stationGetName(stNum));
        glcdDrawRect(canvas->glcd->x, canvas->glcd->y, lt->tuner.bar.barW - nameLen, fh, canvas->pal->bg);
    }
    freqOld = freq;

    // Stereo / forced mono indicator
    bool forcedMono = tuner->par.forcedMono;
    bool stereo = ((tuner->status.flags & TUNER_FLAG_STEREO) == TUNER_FLAG_STEREO);
    static bool stereoOld = false;
    static bool forcedMonoOld = false;
    if (clear || (stereoOld != stereo) || (forcedMonoOld != forcedMono)) {
        icon = glcdFindIcon(forcedMono ? ICON_FMONO : ICON_STEREO, iconSet);
        if (icon) {
            glcdSetXY(lt->rect.w - icon->width, 0);
            glcdDrawImage(icon, (stereo || forcedMono) ?
                          canvas->pal->active : canvas->pal->inactive, canvas->pal->bg);
        }
        stereoOld = stereo;
        forcedMonoOld = forcedMono;
    }

    // RDS enabled indicator
    bool rdsFlag = rdsGetFlag();
    static bool rdsFlagOld = false;
    bool rdsSpClear = (clear || (rdsFlagOld != rdsFlag));
    rdsFlagOld = rdsFlag;

    if (rdsSpClear) {
        icon = glcdFindIcon(ICON_RDS, iconSet);
        if (icon) {
            glcdSetXY(lt->rect.w - icon->width, icon->height + iconSpace);
            glcdDrawImage(icon, rdsFlag ? canvas->pal->active : canvas->pal->inactive, canvas->pal->bg);
        }
        // Clear RDS/Spectrum area
        GlcdRect rect = glcdGetRect();
        glcdDrawRect(0, rect.h / 2, rect.w, rect.h / 2, canvas->pal->bg);
        sp->redraw = true;
    }

    if (rdsFlag) {
        drawRds(rds);
        return;
    }

    rdsReset();
    // Spectrum
    if (!sp->ready) {
        return;
    }

    GlcdRect rect = canvas->glcd->rect;
    rect.h /= 2;
    rect.y = rect.h;
    drawSpectrum(sp, SP_CHAN_BOTH, &rect);

    sp->redraw = false;
    sp->ready = false;
}

void layoutShowTextEdit(bool clear)
{
    TextEdit *te = &canvas->te;
    Glcd *glcd = canvas->glcd;
    const CanvasPalette *pal = canvas->pal;

    const tFont *editFont = lt->textEdit.editFont;
    const int16_t feh = editFont->chars[0].image->height;
    const int16_t few = editFont->chars[0].image->width;
    const GlcdRect *rect = &lt->textEdit.rect;

    const int16_t yPos = (rect->h - feh) / 2;
    const int16_t xRoll = rect->w - few * 3 / 2;

    glcdSetFont(editFont);

    if (clear) {
        glcdSetXY(0, 0);
        glcdSetFontBgColor(pal->inactive);
        glcdSetStringFramed(true);
        glcdWriteStringConst(labelsGet(LABEL_TUNER_FM_STATION_NAME));
        glcdSetStringFramed(false);
        // The rest of space after edit line
        glcdDrawRect(glcd->x, yPos, xRoll - glcd->x, feh, pal->inactive);
    }

    glcdSetXY(0, yPos);

    // String itself
    for (uint16_t i = 0; i < te->uLen; i++) {
        glcdWriteUChar(te->uStr[i]);
        glcdWriteUChar(LETTER_SPACE_CHAR);
    }

    glcdSetFontBgColor(pal->fg);
    glcdWriteUChar(LETTER_SPACE_CHAR);
    glcdSetFontBgColor(pal->bg);

    // The rest of space after edit line
    glcdDrawRect(glcd->x, yPos, xRoll - glcd->x, feh, pal->bg);

    // Gray vertical offset before the roller
    glcdDrawRect(xRoll - few / 4, 0, few / 4, rect->h, pal->inactive);
    // The roller
    for (int8_t i = -2; i <= 2; i++) {
        glcdSetXY(xRoll, yPos + i * feh);
        int16_t sPos = te->sPos + i;
        UChar uCode = ' ';
        if (sPos >= 0 && sPos <= te->lastPos) {
            uCode = editFont->chars[te->sPos + i].code;
        }

        glcdSetFontBgColor(pal->inactive);
        if (i == 0) {
            glcdSetFontColor(pal->fg);
        } else {
            glcdSetFontColor(pal->gray);
        }
        glcdWriteUChar(LETTER_SPACE_CHAR);
        glcdWriteUChar(uCode);
        glcdWriteUChar(LETTER_SPACE_CHAR);

        glcdSetFontColor(pal->fg);
        glcdSetFontBgColor(pal->bg);
        glcdDrawRect(glcd->x, glcd->y, lt->textEdit.rect.w - glcd->x, feh, pal->inactive);
    }
}

void layoutShowTimer(bool clear, int32_t value)
{
    Spectrum *sp = spGet();

    static int8_t hour;
    static int8_t min;
    static int8_t sec;

    if (clear) {
        hour = -1;
        min = -1;
        sec = -1;
    }

    RTC_type rtc;
    rtc.etm = RTC_NOEDIT;

    if (value < 0) {
        rtc.hour = -1;
        rtc.min = -1;
        rtc.sec = -1;
    } else {
        value /= 1000;
        rtc.hour = (int8_t)(value / 3600);
        rtc.min = (int8_t)(value / 60 % 60);
        rtc.sec = (int8_t)(value % 60);
    }

    // HH:MM:SS
    glcdSetFont(lt->time.hmsFont);

    int16_t digW = lt->time.hmsFont->chars[glcdFontSymbolPos('0')].image->width;
    int16_t ltspW = lt->time.hmsFont->chars[glcdFontSymbolPos(LETTER_SPACE_CHAR)].image->width;

    int16_t hmsX = (lt->rect.w - (6 * digW + 15 * ltspW)) /
                   2; // 6 digits HHMMSS + 13 letter spaces + 2 ':'

    if (clear || rtc.hour != hour) {
        glcdSetXY(hmsX, lt->time.hmsY);
        drawTm(&rtc, RTC_HOUR);
    }
    if (clear) {
        glcdWriteUChar(LETTER_SPACE_CHAR);
        glcdWriteUChar(':');
        glcdWriteUChar(LETTER_SPACE_CHAR);
    }

    if (clear || rtc.min != min) {
        glcdSetX(hmsX + 2 * digW + 6 * ltspW);
        drawTm(&rtc, RTC_MIN);
    }
    if (clear) {
        glcdWriteUChar(LETTER_SPACE_CHAR);
        glcdWriteUChar(':');
        glcdWriteUChar(LETTER_SPACE_CHAR);
    }

    if (clear || rtc.sec != sec) {
        glcdSetX(hmsX + 4 * digW + 12 * ltspW);
        drawTm(&rtc, RTC_SEC);
    }

    hour = rtc.hour;
    min = rtc.min;
    sec = rtc.sec;

    // Spectrum

    if (!sp->ready) {
        return;
    }

    GlcdRect rect = canvas->glcd->rect;
    rect.h /= 2;
    rect.y = rect.h;
    drawSpectrum(sp, SP_CHAN_BOTH, &rect);

    sp->redraw = false;
    sp->ready = false;
}
