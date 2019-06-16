#include "canvas.h"
#include <string.h>

#include "../karadio.h"
#include "../menu.h"
#include "../rtc.h"
#include "../settings.h"
#include "../spectrum.h"
#include "../tr/labels.h"
#include "../tuner/rds.h"
#include "../tuner/stations.h"

typedef union {
    RTC_type rtc;
} DrawData;

static DrawData prev;

typedef union {
    struct {
        uint8_t show[SP_CHAN_END];   // Value to show
        uint8_t prev[SP_CHAN_END];   // Previous value
        uint8_t peak[SP_CHAN_END];   // Peak value
        uint8_t fall[SP_CHAN_END];   // Fall speed
    };
    SpectrumColumn col;
} SpCol;

typedef struct {
    SpCol col[SPECTRUM_SIZE];
} SpDrawData;

static const Palette canvasPalette = {
    .fg = LCD_COLOR_WHITE,
    .bg = LCD_COLOR_BLACK,
    .gray = LCD_COLOR_GRAY,
    .spCol = LCD_COLOR_ELECTRIC_BLUE,
    .spPeak = LCD_COLOR_WITCH_HAZE,
    .inactive = LCD_COLOR_NERO,
    .active = LCD_COLOR_AQUA,
};

static Canvas canvas = {
    .pal = &canvasPalette,
};

static SpDrawData spDrawData;

const Layout *layoutGet(void);

void canvasInit(void)
{
    glcdInit(&canvas.glcd);

    canvas.layout = layoutGet();

    bool rotate = settingsRead(PARAM_DISPLAY_ROTATE);
    glcdRotate(rotate);

    canvas.glcd->rect = canvas.layout->rect;

    menuGet()->dispSize = canvas.layout->menu.itemCnt;
}

Canvas *canvasGet(void)
{
    return &canvas;
}

void canvasClear(void)
{
    GlcdRect rect = glcdGetRect();

    glcdDrawRect(0, 0, rect.w, rect.h, canvas.pal->bg);
    glcdShift(0);

    const int16_t ft = canvas.glcd->drv->height / 100;

    if (rect.x >= 2 * ft && rect.y >= 2 * ft) {
        glcdDrawFrame(-ft, -ft, rect.w + 2 * ft, rect.h + 2 * ft, ft, canvas.pal->bg);
        glcdDrawFrame(-2 * ft, -2 * ft, rect.w + 4 * ft, rect.h + 4 * ft, ft, canvas.pal->fg);
    }

    glcdSetFontColor(canvas.pal->fg);
    glcdSetFontBgColor(canvas.pal->bg);

    memset(&prev, 0, sizeof(prev));
}

static void drawMenuItem(uint8_t idx, const tFont *fontItem);
static uint8_t calcSpCol(Spectrum *sp, int16_t chan, int16_t scale, uint8_t col,
                         SpectrumColumn *spCol);
static void drawWaterfall(Spectrum *sp);
static void drawSpectrum(Spectrum *sp, SpChan chan, GlcdRect *rect);
static void drawRds(Rds *rds);

static void drawTmSpacer(char spacer, bool clear)
{
    if (clear) {
        glcdWriteUChar(LETTER_SPACE_CHAR);
        glcdWriteUChar(spacer);
        glcdWriteUChar(LETTER_SPACE_CHAR);
    }
}

static void drawTm(RTC_type *rtc, RtcMode tm, bool clear)
{
    int8_t time = *((int8_t *)rtc + tm);
    int8_t oldTime = *((int8_t *)&prev.rtc + tm);

    bool etmChange = ((rtc->etm != prev.rtc.etm) &&
                      (rtc->etm == tm || prev.rtc.etm == tm));

    if (clear || time != oldTime || etmChange) {
        glcdSetFontColor(canvas.pal->fg);
        glcdSetFontBgColor(canvas.pal->bg);
        if (rtc->etm == tm) {
            glcdSetFontColor(canvas.pal->bg);
            glcdSetFontBgColor(canvas.pal->fg);
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
        glcdSetFontColor(canvas.pal->fg);
        glcdSetFontBgColor(canvas.pal->bg);
    }
}

static void drawMenuItem(uint8_t idx, const tFont *fontItem)
{
    const Layout *lt = canvas.layout;

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
    glcdDrawFrame(0, y_pos, width, ih, 1, active ? canvas.pal->fg : canvas.pal->bg);

    // Draw menu name
    glcdSetFont(fontItem);
    glcdSetFontColor(canvas.pal->fg);

    glcdSetXY(4, y_pos + 2);
    if (menu->list[idx] != MENU_NULL) {
        glcdWriteString("  ");
    } else {
        glcdWriteString("< ");
    }
    glcdWriteStringConst(name);

    // Draw menu value
    int16_t x = canvas.glcd->x;
    glcdSetXY(width - 2, y_pos + 2);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);

    // Inverse value color if selected
    uint16_t color = canvas.glcd->font.color;
    uint16_t bgColor = canvas.glcd->font.bgColor;
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
    glcdDrawRect(x, y_pos + 2, width - 2 - x - strLen, fIh, canvas.pal->bg);
}

static uint8_t calcSpCol(Spectrum *sp, int16_t chan, int16_t scale, uint8_t col,
                         SpectrumColumn *spCol)
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
        *spCol = spDrawCol->col;
    } else {
        raw = sp->data[chan].raw[col];
        spCol->showW = spDrawCol->show[chan];
        spCol->prevW = spDrawCol->prev[chan];
        spCol->peakW = spDrawCol->peak[chan];
        spCol->fallW = spDrawCol->fall[chan];
    }

    raw = (scale * raw) >> 8; // / N_DB = 256

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
        spDrawCol->col = *spCol;
    } else {
        spDrawCol->show[chan] = (uint8_t)spCol->showW;
        spDrawCol->prev[chan] = (uint8_t)spCol->prevW;
        spDrawCol->peak[chan] = (uint8_t)spCol->peakW;
        spDrawCol->fall[chan] = (uint8_t)spCol->fallW;
    }

    return (uint8_t)raw;
}

static void drawWaterfall(Spectrum *sp)
{
    const Layout *lt = canvas.layout;

    if (++sp->wtfX >= lt->rect.w) {
        sp->wtfX = 0;
    }
    glcdShift((sp->wtfX + 1) % lt->rect.w);

    for (uint8_t col = 0; col < SPECTRUM_SIZE; col++) {
        SpectrumColumn spCol;
        calcSpCol(sp, SP_CHAN_BOTH, 224, col, &spCol);
        uint16_t color = glcdGetRainbowColor((uint8_t)spCol.showW);

        int16_t posCurr = (col * lt->rect.h) / SPECTRUM_SIZE;
        int16_t posNext = ((col + 1) * lt->rect.h) / SPECTRUM_SIZE;

        int16_t wfH = (posNext - posCurr);
        if (wfH) {
            glcdDrawRect(sp->wtfX, lt->rect.h - posCurr - wfH, 1, wfH, color);
        }
    }
}

static void drawSpectrum(Spectrum *sp, SpChan chan, GlcdRect *rect)
{
    const int16_t step = (rect->w  + 1) / SPECTRUM_SIZE + 1;    // Step of columns
    const int16_t colW = step - (step / 2);                     // Column width
    const int16_t num = (rect->w + colW - 1) / step;            // Number of columns

    const int16_t width = (num - 1) * step + colW;              // Width of spectrum
    const int16_t height = rect->h;                             // Height of spectrum

    const int16_t oft = (rect->w - width) / 2;                  // Spectrum offset for symmetry

    const int16_t y = rect->y;

    if (sp->redraw) {
        memset(&spDrawData, 0, sizeof (SpDrawData));
        memset(sp->data, 0, sizeof (SpData) * SP_CHAN_END);
    }

    for (uint8_t col = 0; col < num; col++) {
        int16_t x = oft + col * step;

        SpectrumColumn spCol;
        calcSpCol(sp, chan, height, col, &spCol);
        if (!sp->peaks) {
            spCol.peakW = 0;
        }
        GlcdRect rect = {x, y, colW, height};
        spectrumColumnDraw(&spCol, &rect, sp->redraw);
    }
}

static void drawRds(Rds *rds)
{
    const Layout *lt = canvas.layout;

    glcdSetFont(lt->rds.psFont);
    glcdSetXY(0, lt->rect.h / 2);
    uint16_t nameLen = glcdWriteString(rds->PS);

    glcdDrawRect(canvas.glcd->x, canvas.glcd->y,
                 lt->tuner.bar.barW - nameLen, lt->tuner.nameFont->chars[0].image->height,
                 canvas.pal->bg);


    glcdSetFont(lt->rds.textFont);
    glcdSetXY(0, lt->rect.h / 2 + lt->tuner.nameFont->chars[0].image->height);
    glcdWriteString(rds->text);
}


void canvasShowTime(bool clear)
{
    const Layout *lt = canvas.layout;

    RTC_type rtcStruct; // TODO: Use one struct in rtc driver
    rtcStruct.etm = rtcGetMode();

    RTC_type *rtc = &rtcStruct;

    rtcGetTime(rtc);

    // HH:MM:SS
    glcdSetFont(lt->time.hmsFont);

    int16_t digW = lt->time.hmsFont->chars[glcdFontSymbolPos('0')].image->width;
    int16_t ltspW = lt->time.hmsFont->chars[glcdFontSymbolPos(LETTER_SPACE_CHAR)].image->width;

    int16_t timeLen = 6 * digW + 15 * ltspW;    // 6 digits HHMMSS + 13 letter spaces + 2 ':'
    int16_t timeX = (lt->rect.w - (timeLen)) / 2;

    glcdSetXY(timeX, lt->time.hmsY);
    drawTm(rtc, RTC_HOUR, clear);

    drawTmSpacer(':', clear);

    timeX += digW * 2 + ltspW * 6;
    glcdSetXY(timeX, lt->time.hmsY);
    drawTm(rtc, RTC_MIN, clear);

    drawTmSpacer(':', clear);

    timeX += digW * 2 + ltspW * 6;
    glcdSetXY(timeX, lt->time.hmsY);
    drawTm(rtc, RTC_SEC, clear);

    // DD:MM:YYYY
    glcdSetFont(lt->time.dmyFont);

    digW = lt->time.dmyFont->chars[glcdFontSymbolPos('0')].image->width;
    ltspW = lt->time.dmyFont->chars[glcdFontSymbolPos(LETTER_SPACE_CHAR)].image->width;
    timeLen = 8 * digW + 17 * ltspW;            // 8 digits HHMMSS + 15 letter spaces + 2 '.'

    timeX = (lt->rect.w - timeLen) / 2;
    glcdSetXY(timeX, lt->time.dmyY);
    drawTm(rtc, RTC_DATE, clear);

    drawTmSpacer('.', clear);

    timeX += digW * 2 + ltspW * 6;
    glcdSetXY(timeX, lt->time.dmyY);
    drawTm(rtc, RTC_MONTH, clear);

    drawTmSpacer('.', clear);

    timeX += digW * 2 + ltspW * 6;
    glcdSetXY(timeX, lt->time.dmyY);
    drawTm(rtc, RTC_YEAR, clear);

    // Weekday
    glcdSetFont(lt->time.wdFont);
    glcdSetFontColor(canvas.pal->fg);

    if (clear || rtc->wday != prev.rtc.wday) {
        // Clear the area with weekday label
        glcdDrawRect(0, lt->time.wdY, lt->rect.w,
                     (int16_t)lt->time.wdFont->chars[0].image->height, canvas.pal->bg);

        const char *wdayLabel = labelsGet((Label)(LABEL_SUNDAY + rtc->wday));
        glcdSetXY(lt->rect.w / 2, lt->time.wdY);
        glcdSetFontAlign(FONT_ALIGN_CENTER);
        glcdWriteStringConst(wdayLabel);
    }

    prev.rtc = *rtc;
}

void canvasShowMenu(bool clear)
{
    (void)clear;

    const Layout *lt = canvas.layout;

    Menu *menu = menuGet();

    const int16_t fHh = (int16_t)lt->menu.headFont->chars[0].image->height;
    const int16_t fIh = (int16_t)lt->menu.menuFont->chars[0].image->height;
    const uint8_t items = menu->dispSize;

    const int16_t dividerPos = (lt->rect.h - (fIh + 4) * items + fHh) / 2;

    // Show header
    const char *parentName = menuGetName(menu->parent);
    glcdSetFont(lt->menu.headFont);
    glcdSetFontColor(canvas.pal->fg);

    glcdSetXY(2, 0);
    glcdWriteStringConst(parentName);
    // Fill free space after header
    glcdDrawRect(canvas.glcd->x, canvas.glcd->y, lt->rect.w - canvas.glcd->x, fHh, canvas.pal->bg);

    glcdDrawRect(0, dividerPos, lt->rect.w, 1, canvas.glcd->font.color);

    for (uint8_t idx = 0; idx < menu->listSize; idx++) {
        if (idx >= menu->dispOft && idx < items + menu->dispOft) {
            drawMenuItem(idx, lt->menu.menuFont);
        }
    }
}

void canvasShowTune(bool clear)
{
    const Layout *lt = canvas.layout;

    const tFont *iconSet = lt->iconSet;

    Spectrum *sp = spGet();
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    const char *label = labelsGet(LABEL_IN_TUNER + inType);
    Icon icon = (ICON_TUNER + inType);

    if (aProc->tune < AUDIO_TUNE_GAIN) {
        label = labelsGet(LABEL_VOLUME + aProc->tune);
        icon = ICON_VOLUME + aProc->tune;
    }

    if (icon == ICON_VOLUME && aProc->par.mute) {
        icon = ICON_MUTE_ON;
    }

    const int16_t value = aProc->par.item[aProc->tune].value;

    const AudioGrid *grid = aProc->par.item[aProc->tune].grid;
    const int8_t min = grid ? grid->min : 0;
    const int8_t max = grid ? grid->max : 0;
    const uint8_t mStep = grid ? grid->mStep : 0;

    if (clear) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);
        glcdWriteStringConst(label);
    }

    static Icon iconOld = ICON_VOLUME;
    if (clear || iconOld != icon) {
        // Icon
        glcdSetXY(lt->rect.w - iconSet->chars[0].image->width, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas.pal->fg, canvas.pal->bg);
    }
    iconOld = icon;

    static int16_t valueOld;
    if (clear || valueOld != value) {
        // Bar
        StripedBar bar = {value, min, max};
        stripedBarDraw(&bar, &lt->tune.bar, clear);

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

    GlcdRect rect = canvas.glcd->rect;
    rect.h /= 2;
    rect.y = rect.h;
    drawSpectrum(sp, SP_CHAN_BOTH, &rect);

    sp->redraw = false;
    sp->ready = false;
}

void canvasShowAudioFlag(bool clear)
{
    const Layout *lt = canvas.layout;

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
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);
        glcdWriteStringConst(labelsGet(label));
    }

    // Icon
    static Icon iconOld = ICON_MUTE_OFF;
    if (clear || icon != iconOld) {
        glcdSetXY(lt->rect.w - iconSet->chars[0].image->width, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas.pal->fg, canvas.pal->bg);
        iconOld = icon;
    }

    // Spectrum
    Spectrum *sp = spGet();

    if (!sp->ready) {
        return;
    }

    GlcdRect rect = canvas.glcd->rect;
    rect.h /= 2;
    rect.y = rect.h;
    drawSpectrum(sp, SP_CHAN_BOTH, &rect);

    sp->redraw = false;
    sp->ready = false;
}

void canvasShowSpectrum(bool clear)
{
    (void)clear;

    Spectrum *sp = spGet();

    if (!sp->ready) {
        return;
    }

    GlcdRect rect = canvas.glcd->rect;

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

void canvasShowTuner(bool clear)
{
    const Layout *lt = canvas.layout;

    Tuner *tuner = tunerGet();
    Spectrum *sp = spGet();
    Rds *rds = rdsGet();

    const tFont *iconSet = lt->iconSet;

    const tImage *icon = NULL;;
    const uint8_t iconSpace = lt->tuner.iconSpace;
    const LayoutStripedBar *ltTunerBar = &lt->tuner.bar;

    // Frequency
    uint16_t freq = tuner->status.freq;
    static uint16_t freqOld = 0;

    if (clear || freqOld != freq) {
        int16_t freqMin = (int16_t)tuner->par.fMin;
        int16_t freqMax = (int16_t)tuner->par.fMax;

        const tFont *fmFont = lt->lblFont;

        glcdSetFont(fmFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);

        glcdWriteString("FM ");

        glcdWriteNum(freq / 100, 3, ' ', 10);
        glcdWriteUChar(LETTER_SPACE_CHAR);
        glcdWriteUChar('.');
        glcdWriteUChar(LETTER_SPACE_CHAR);
        glcdWriteNum(freq % 100, 2, '0', 10);

        // Scale
        StripedBar bar = {(int16_t)freq, freqMin, freqMax};
        stripedBarDraw(&bar, ltTunerBar, clear);

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
        glcdDrawRect(canvas.glcd->x, canvas.glcd->y, lt->tuner.bar.barW - nameLen, fh, canvas.pal->bg);
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
                          canvas.pal->active : canvas.pal->inactive, canvas.pal->bg);
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
            glcdDrawImage(icon, rdsFlag ? canvas.pal->active : canvas.pal->inactive, canvas.pal->bg);
        }
        // Clear RDS/Spectrum area
        GlcdRect rect = glcdGetRect();
        glcdDrawRect(0, rect.h / 2, rect.w, rect.h / 2, canvas.pal->bg);
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

    GlcdRect rect = canvas.glcd->rect;
    rect.h /= 2;
    rect.y = rect.h;
    drawSpectrum(sp, SP_CHAN_BOTH, &rect);

    sp->redraw = false;
    sp->ready = false;
}

void canvasShowKaradio(bool clear)
{
    const Layout *lt = canvas.layout;

    const tFont *iconSet = lt->iconSet;
    const char *label = labelsGet(LABEL_IN_KARADIO);

    Icon icon = ICON_KARADIO;

    if (clear) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);
        glcdWriteStringConst(label);
    }

    if (clear) {
        // Icon
        glcdSetXY(lt->rect.w - iconSet->chars[0].image->width, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas.pal->fg, canvas.pal->bg);
    }

    int16_t yPos = iconSet->chars[0].image->height;

    KaRadioData *krData = karadioGet();
    uint16_t nameLen;

    if (clear || (krData->flags & KARADIO_FLAG_NAME)) {

        krData->flags &= ~KARADIO_FLAG_NAME;
        glcdSetFont(lt->rds.psFont);
        glcdSetXY(0, yPos);
        if (krData->playing) {
            nameLen = glcdWriteString(krData->name);
        } else {
            nameLen = 0;
        }
        glcdDrawRect(canvas.glcd->x, canvas.glcd->y,
                     lt->rect.w - nameLen, lt->tuner.nameFont->chars[0].image->height,
                     canvas.pal->bg);
    }

    yPos += lt->rds.psFont->chars[0].image->height;

    if (clear || (krData->flags & KARADIO_FLAG_META)) {
        krData->flags &= ~KARADIO_FLAG_META;
        glcdSetFont(lt->rds.textFont);
        glcdSetXY(0, yPos);
        if (krData->playing) {
            nameLen = glcdWriteString(krData->meta);
        } else {
            nameLen = 0;
        }
        glcdDrawRect(canvas.glcd->x, canvas.glcd->y,
                     lt->rect.w - nameLen, lt->tuner.nameFont->chars[0].image->height,
                     canvas.pal->bg);
    }

    yPos += lt->tuner.nameFont->chars[0].image->height;

    Spectrum *sp = spGet();
    // Spectrum
    if (!sp->ready) {
        return;
    }

    GlcdRect rect = canvas.glcd->rect;
    rect.y = yPos;
    rect.h = rect.h - rect.y;
    drawSpectrum(sp, SP_CHAN_BOTH, &rect);

    sp->redraw = false;
    sp->ready = false;
}

void canvasShowAudioInput(bool clear, Icon icon)
{
    const Layout *lt = canvas.layout;

    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    if (inType == IN_TUNER && !aProc->par.mute) {
        canvasShowTuner(clear);
        return;
    }

    if (inType == IN_KARADIO && !aProc->par.mute) {
        canvasShowKaradio(clear);
        return;
    }

    const tFont *iconSet = lt->iconSet;
    const char *label = labelsGet(LABEL_IN_TUNER + inType);

    if (icon == ICON_EMPTY) {
        icon = (ICON_TUNER + inType);
    }

    if (aProc->par.mute) {
        icon = ICON_MUTE_ON;
    }

    if (clear) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);
        glcdWriteStringConst(label);
    }

    static Icon iconOld = ICON_VOLUME;
    if (clear || iconOld != icon) {
        // Icon
        glcdSetXY(lt->rect.w - iconSet->chars[0].image->width, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas.pal->fg, canvas.pal->bg);
    }
    iconOld = icon;

    Spectrum *sp = spGet();
    // Spectrum
    if (!sp->ready) {
        return;
    }

    GlcdRect rect = canvas.glcd->rect;
    rect.y = lt->lblFont->chars[0].image->height;
    rect.h = (rect.h - rect.y) / 2;
    drawSpectrum(sp, SP_CHAN_LEFT, &rect);
    rect.y += rect.h;
    drawSpectrum(sp, SP_CHAN_RIGHT, &rect);

    sp->redraw = false;
    sp->ready = false;
}

void canvasShowTextEdit(bool clear)
{
    const Layout *lt = canvas.layout;

    TextEdit *te = &canvas.te;

    textEditDraw(te, &lt->textEdit, clear);
}

void canvasShowTimer(bool clear, int32_t value)
{
    const Layout *lt = canvas.layout;

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

    int16_t timeLen = 6 * digW + 15 * ltspW;    // 6 digits HHMMSS + 13 letter spaces + 2 ':'
    int16_t timeX = (lt->rect.w - timeLen) / 2;

    glcdSetXY(timeX, lt->time.hmsY);
    drawTm(&rtc, RTC_HOUR, clear);

    drawTmSpacer(':', clear);

    timeX += digW * 2 + ltspW * 6;
    glcdSetX(timeX);
    drawTm(&rtc, RTC_MIN, clear);

    drawTmSpacer(':', clear);

    timeX += digW * 2 + ltspW * 6;
    glcdSetX(timeX);
    drawTm(&rtc, RTC_SEC, clear);

    prev.rtc = rtc;

    // Spectrum
    Spectrum *sp = spGet();

    if (!sp->ready) {
        return;
    }

    GlcdRect rect = canvas.glcd->rect;
    rect.h /= 2;
    rect.y = rect.h;
    drawSpectrum(sp, SP_CHAN_BOTH, &rect);

    sp->redraw = false;
    sp->ready = false;
}
