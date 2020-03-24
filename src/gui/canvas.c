#include "canvas.h"

#include <stdlib.h>
#include <string.h>

#include "amp.h"
#include "bt.h"
#include "karadio.h"
#include "menu.h"
#include "rtc.h"
#include "settings.h"
#include "spectrum.h"
#include "swtimers.h"
#include "tr/labels.h"
#include "tuner/rds.h"
#include "tuner/stations.h"
#include "utils.h"

#define SPECTRUM_SIZE   128

typedef union {
    RTC_type rtc;
    int16_t wtfX;
    struct {
        uint16_t freq;
        int16_t value;
        Icon icon;
        bool stereo;
        bool forcedMono;
        bool rdsFlag;
    } par;
} DrawData;

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

typedef struct {
    uint8_t raw[SPECTRUM_SIZE];
} SpData;


static void drawMenuItem(uint8_t idx, const tFont *fontItem);
static void calcSpCol(int16_t chan, int16_t scale, uint8_t col, SpectrumColumn *spCol);
static void drawWaterfall(bool clear);
static void drawSpectrum(bool clear, bool check, bool mirror, SpChan chan, GlcdRect *rect);
static void drawSpectrumMode(bool clear, GlcdRect rect);
static void drawRds(Rds *rds);
static bool checkSpectrumReady(void);
static void fftGet128(FftSample *sp, uint8_t *out, size_t size);


static Canvas canvas;
static SpDrawData spDrawData;
static SpData spData[SP_CHAN_END];
static DrawData prev;


static const tImage *glcdFindIcon(Icon code, const tFont *iFont)
{
    const tImage *ret = NULL;

    // Find icon pos
    int32_t iPos = -1;
    for (int16_t i = 0; i < iFont->length; i++) {
        if (iFont->chars[i].code == (int32_t)code) {
            iPos = i;
            break;
        }
    }

    if (iPos >= 0) {
        ret = iFont->chars[iPos].image;
    }

    return  ret;
}

void canvasInit(void)
{
    glcdInit();

    canvas.glcd = glcdGet();
    canvas.layout = layoutGet();

    PalIdx palIdx = (PalIdx)settingsGet(PARAM_DISPLAY_PALETTE);
    paletteSetIndex(palIdx);
    canvas.pal = paletteGet(palIdx);

    bool rotate = settingsGet(PARAM_DISPLAY_ROTATE);
    glcdRotate(rotate);

    glcdDrawRect(0, 0, dispdrv.width, dispdrv.height, canvas.pal->bg);

    canvas.glcd->rect = canvas.layout->rect;

    menuGet()->dispSize = canvas.layout->menu.itemCnt;
}

Canvas *canvasGet(void)
{
    return &canvas;
}

void canvasClear(void)
{
    GlcdRect rect = canvas.glcd->rect;

    glcdDrawRect(0, 0, rect.w, rect.h, canvas.pal->bg);
    glcdShift(0);

    glcdSetFontColor(canvas.pal->fg);
    glcdSetFontBgColor(canvas.pal->bg);

    memset(&prev, 0, sizeof(prev));
}

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
            glcdWriteString(utilMkStr("%02d", time));
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

    glcdWriteString(menuGetName(menuIdx));

    // Draw menu value
    int16_t x = canvas.glcd->x;
    glcdSetXY(width - 2, y_pos + 2);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);

    // Inverse value color if selected
    color_t color = canvas.glcd->fontFg;
    color_t bgColor = canvas.glcd->fontBg;
    if (active && menu->selected) {
        glcdSetFontColor(bgColor);
        glcdSetFontBgColor(color);
    }
    glcdSetStringFramed(true);
    uint16_t strLen = glcdWriteString(menuGetValueStr(menuIdx));
    glcdSetStringFramed(false);

    glcdSetFontColor(color);
    glcdSetFontBgColor(bgColor);

    // Fill space between name and value
    glcdDrawRect(x, y_pos + 2, width - 2 - x - strLen, fIh, canvas.pal->bg);
}

static void calcSpCol(int16_t chan, int16_t scale, uint8_t col, SpectrumColumn *spCol)
{
    int16_t raw;

    SpCol *spDrawCol = &spDrawData.col[col];

    if (chan == SP_CHAN_BOTH) {
        uint8_t rawL = spData[SP_CHAN_LEFT].raw[col];
        uint8_t rawR = spData[SP_CHAN_RIGHT].raw[col];
        if (rawL > rawR) {
            raw = rawL;
        } else {
            raw = rawR;
        }
        *spCol = spDrawCol->col;
    } else {
        raw = spData[chan].raw[col];
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
}

static color_t getRainbowColor(uint8_t value)
{
    color_t color = 0xFFFF;

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

static void drawWaterfall(bool clear)
{
    if (!checkSpectrumReady()) {
        return;
    }
    spGetADC(SP_CHAN_LEFT, spData[SP_CHAN_LEFT].raw, SPECTRUM_SIZE, fftGet128);
    spGetADC(SP_CHAN_RIGHT, spData[SP_CHAN_RIGHT].raw, SPECTRUM_SIZE, fftGet128);

    const Layout *lt = canvas.layout;

    if (clear || ++prev.wtfX >= lt->rect.w) {
        prev.wtfX = 0;
    }
    glcdShift((prev.wtfX + 1) % lt->rect.w);

    for (uint8_t col = 0; col < SPECTRUM_SIZE; col++) {
        SpectrumColumn spCol;
        calcSpCol(SP_CHAN_BOTH, 224, col, &spCol);
        color_t color = getRainbowColor((uint8_t)spCol.showW);

        int16_t posCurr = (col * lt->rect.h) / SPECTRUM_SIZE;
        int16_t posNext = ((col + 1) * lt->rect.h) / SPECTRUM_SIZE;

        int16_t wfH = (posNext - posCurr);
        if (wfH) {
            glcdDrawRect(prev.wtfX, lt->rect.h - posCurr - wfH, 1, wfH, color);
        }
    }
}

static void calcGradient(Spectrum *sp, int16_t height, bool mirror, color_t *grad)
{
    color_t colorB = mirror ? canvas.pal->spColG : canvas.pal->spColB;
    color_t colorG = mirror ? canvas.pal->spColB : canvas.pal->spColG;

    color_t rB = (colorB & 0xF800) >> 11;
    color_t gB = (colorB & 0x07E0) >> 5;
    color_t bB = (colorB & 0x001F) >> 0;

    color_t rG = (colorG & 0xF800) >> 11;
    color_t gG = (colorG & 0x07E0) >> 5;
    color_t bG = (colorG & 0x001F) >> 0;

    for (int16_t i = 0; i < height; i++) {
        if (sp->grad) {
            grad[i] = (color_t)(((rB + (rG - rB) * i / (height - 1)) << 11) |
                                ((gB + (gG - gB) * i / (height - 1)) << 5) |
                                ((bB + (bG - bB) * i / (height - 1)) << 0));
        } else {
            grad[i] = colorB;
        }
    }
}

static bool checkSpectrumReady(void)
{
    if (swTimGet(SW_TIM_SP_CONVERT) == 0) {
        swTimSet(SW_TIM_SP_CONVERT, 20);
        return true;
    }

    return false;
}

static void fftGet128(FftSample *sp, uint8_t *out, size_t size)
{
    uint8_t db;
    uint8_t *po = out;

    memset(po, 0, size);

    for (int16_t i = 0; i < FFT_SIZE / 2; i++) {
        uint16_t calc = (uint16_t)((sp[i].fr * sp[i].fr + sp[i].fi * sp[i].fi) >> 15);

        db = spGetDb(calc, 0, N_DB - 1);

        if (*po < db) {
            *po = db;
        }

        if ((i < 48) ||
            ((i < 96) && (i & 0x01) == 0x01) ||
            ((i < 192) && (i & 0x03) == 0x03) ||
            ((i < 384) && (i & 0x07) == 0x07) ||
            ((i & 0x0F) == 0x0F)) {
            po++;

            if (--size == 0) {
                break;
            }
        }
    }
}

static void drawSpectrum(bool clear, bool check, bool mirror, SpChan chan, GlcdRect *rect)
{
    if (check && !checkSpectrumReady()) {
        return;
    }

    if (chan == SP_CHAN_LEFT || chan == SP_CHAN_BOTH) {
        spGetADC(SP_CHAN_LEFT, spData[SP_CHAN_LEFT].raw, SPECTRUM_SIZE, fftGet128);
    }
    if (chan == SP_CHAN_RIGHT || chan == SP_CHAN_BOTH) {
        spGetADC(SP_CHAN_RIGHT, spData[SP_CHAN_RIGHT].raw, SPECTRUM_SIZE, fftGet128);
    }

    const int16_t step = (rect->w  + 1) / SPECTRUM_SIZE + 1;    // Step of columns
    const int16_t colW = step - (step / 2);                     // Column width
    const int16_t num = (rect->w + colW - 1) / step;            // Number of columns

    const int16_t width = (num - 1) * step + colW;              // Width of spectrum
    const int16_t height = rect->h;                             // Height of spectrum

    const int16_t oft = (rect->w - width) / 2;                  // Spectrum offset for symmetry

    const int16_t y = rect->y;

    if (clear) {
        memset(&spDrawData, 0, sizeof (SpDrawData));
        memset(spData, 0, sizeof (SpData) * SP_CHAN_END);
    }

    Spectrum *sp = spGet();

    color_t *grad = NULL;

    if (sp->grad) {
        grad = malloc((size_t)height * sizeof (color_t));
        calcGradient(sp, height, mirror, grad);
    }

    for (uint8_t col = 0; col < num; col++) {
        int16_t x = oft + col * step;

        SpectrumColumn spCol;
        calcSpCol(chan, height, col, &spCol);
        if (!sp->peaks) {
            spCol.peakW = 0;
        }
        GlcdRect rect = {x, y, colW, height};
        spectrumColumnDraw(&spCol, &rect, clear, mirror, grad);
    }

    free(grad);
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
        glcdSetFontAlign(GLCD_ALIGN_CENTER);
        glcdWriteString(wdayLabel);
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
    glcdWriteString(parentName);
    // Fill free space after header
    glcdDrawRect(canvas.glcd->x, canvas.glcd->y, lt->rect.w - canvas.glcd->x, fHh, canvas.pal->bg);

    glcdDrawRect(0, dividerPos, lt->rect.w, 1, canvas.glcd->fontFg);

    for (uint8_t idx = 0; idx < menu->listSize; idx++) {
        if (idx >= menu->dispOft && idx < items + menu->dispOft) {
            drawMenuItem(idx, lt->menu.menuFont);
        }
    }
}

void canvasShowTune(bool clear)
{
    AudioProc *aProc = audioGet();

    if (NULL == aProc->api) {
        canvasShowSpectrum(clear);
        return;
    }

    const Layout *lt = canvas.layout;
    const tFont *iconSet = lt->iconSet;

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

    const int16_t value = aProc->par.tune[aProc->tune].value;

    const AudioGrid *grid = aProc->par.tune[aProc->tune].grid;
    const int8_t min = grid ? grid->min : 0;
    const int8_t max = grid ? grid->max : 0;
    const int8_t mStep = grid ? grid->mStep : 0;

    if (clear) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);
        glcdWriteString(label);
    }

    if (clear || icon != prev.par.icon) {
        // Icon
        glcdSetXY(lt->rect.w - iconSet->chars[0].image->width, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas.pal->fg, canvas.pal->bg);
        prev.par.icon = icon;
    }

    if (clear || value != prev.par.value) {
        // Bar
        StripedBar bar = {value, min, max};
        stripedBarDraw(&bar, &lt->tune.bar, clear);

        // Value
        glcdSetXY(lt->rect.w, lt->tune.valY);
        glcdSetFontAlign(GLCD_ALIGN_RIGHT);
        glcdSetFont(lt->tune.valFont);
        glcdWriteString(utilMkStr("%3d", value * mStep / STEP_MULT));
        prev.par.value = value;
    }

    int16_t yPos = lt->tune.valY + lt->tune.valFont->chars[0].image->height;

    GlcdRect rect = canvas.glcd->rect;
    rect.y = yPos;
    rect.h = rect.h - rect.y;
    drawSpectrumMode(clear, rect);
}

void canvasShowAudioFlag(bool clear)
{
    AudioProc *aProc = audioGet();

    if (NULL == aProc->api) {
        canvasShowSpectrum(clear);
        return;
    }

    const Layout *lt = canvas.layout;
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
        glcdWriteString(labelsGet(label));
    }

    // Icon
    if (clear || icon != prev.par.icon) {
        glcdSetXY(lt->rect.w - iconSet->chars[0].image->width, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas.pal->fg, canvas.pal->bg);
        prev.par.icon = icon;
    }

    int16_t yPos = lt->lblFont->chars[0].image->height;

    // Spectrum
    GlcdRect rect = canvas.glcd->rect;
    rect.y = yPos;
    rect.h = rect.h - rect.y;
    drawSpectrumMode(clear, rect);
}

static void drawSpectrumMode(bool clear, GlcdRect rect)
{
    Spectrum *sp = spGet();

    switch (sp->mode) {
    case SP_MODE_STEREO:
    case SP_MODE_MIRROR:
    case SP_MODE_ANTIMIRROR:
        rect.h = rect.h / 2;
        drawSpectrum(clear, true, sp->mode == SP_MODE_ANTIMIRROR, SP_CHAN_LEFT, &rect);
        rect.y += rect.h;
        drawSpectrum(clear, false, sp->mode == SP_MODE_MIRROR, SP_CHAN_RIGHT, &rect);
        break;
    default:
        drawSpectrum(clear, true, false, SP_CHAN_BOTH, &rect);
        break;
    }
}

void canvasShowSpectrum(bool clear)
{
    Spectrum *sp = spGet();

    switch (sp->mode) {
    case SP_MODE_STEREO:
    case SP_MODE_MIRROR:
    case SP_MODE_ANTIMIRROR:
    case SP_MODE_MIXED:
        drawSpectrumMode(clear, glcdGet()->rect);
        break;
    case SP_MODE_WATERFALL:
        drawWaterfall(clear);
        break;
    default:
        break;
    }
}

void canvasShowTuner(bool clear)
{
    const Layout *lt = canvas.layout;

    Tuner *tuner = tunerGet();
    Rds *rds = rdsGet();

    const tFont *iconSet = lt->iconSet;

    const tImage *icon = NULL;
    const uint8_t iconSpace = lt->tuner.iconSpace;
    const LayoutStripedBar *ltTunerBar = &lt->tuner.bar;

    // Frequency
    uint16_t freq = tuner->status.freq;

    if (clear || freq != prev.par.freq) {
        prev.par.freq = freq;

        int16_t freqMin = (int16_t)tuner->par.fMin;
        int16_t freqMax = (int16_t)tuner->par.fMax;

        const tFont *fmFont = lt->lblFont;

        glcdSetFont(fmFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);

        glcdWriteString(utilMkStr("FM %3d.%02d", freq / 100, freq % 100));

        // Scale
        StripedBar bar = {(int16_t)freq, freqMin, freqMax};
        stripedBarDraw(&bar, ltTunerBar, clear);

        glcdSetFont(lt->tuner.stFont);
        glcdSetXY(lt->rect.w, lt->tune.valY);
        glcdSetFontAlign(GLCD_ALIGN_RIGHT);

        // Station number
        int8_t stNum = stationGetNum(freq);
        if (stNum >= 0) {
            glcdWriteString(utilMkStr("%2d", stNum));
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

    // Stereo / forced mono indicator
    bool forcedMono = tuner->par.forcedMono;
    bool stereo = ((tuner->status.flags & TUNER_FLAG_STEREO) == TUNER_FLAG_STEREO);

    if (clear || (stereo != prev.par.stereo) || (forcedMono != prev.par.forcedMono)) {
        prev.par.stereo = stereo;
        prev.par.forcedMono = forcedMono;
        icon = glcdFindIcon(forcedMono ? ICON_FMONO : ICON_STEREO, iconSet);
        if (icon) {
            glcdSetXY(lt->rect.w - icon->width, 0);
            glcdDrawImage(icon, (stereo || forcedMono) ?
                          canvas.pal->active : canvas.pal->inactive, canvas.pal->bg);
        }
    }

    // RDS enabled indicator
    bool rdsFlag = rdsGetFlag();

    bool rdsSpClear = (clear || (rdsFlag != prev.par.rdsFlag));
    prev.par.rdsFlag = rdsFlag;

    if (rdsSpClear) {
        icon = glcdFindIcon(ICON_RDS, iconSet);
        if (icon) {
            glcdSetXY(lt->rect.w - icon->width, icon->height + iconSpace);
            glcdDrawImage(icon, rdsFlag ? canvas.pal->active : canvas.pal->inactive, canvas.pal->bg);
        }
        // Clear RDS/Spectrum area
        GlcdRect rect = canvas.glcd->rect;
        glcdDrawRect(0, rect.h / 2, rect.w, rect.h / 2, canvas.pal->bg);
        clear = true;
    }

    if (rdsFlag) {
        drawRds(rds);
        return;
    }

    rdsReset();

    // Spectrum
    GlcdRect rect = canvas.glcd->rect;
    rect.h /= 2;
    rect.y = rect.h;
    drawSpectrumMode(clear, rect);
}

void canvasShowKaradio(bool clear)
{
    const Layout *lt = canvas.layout;

    const tFont *iconSet = lt->iconSet;
    const char *label = labelsGet(LABEL_IN_KARADIO);

    Icon icon = ICON_KARADIO;

    if (clear) {
        // Icon
        glcdSetXY(lt->rect.w - iconSet->chars[0].image->width, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas.pal->fg, canvas.pal->bg);
    }

    KaRadio *krData = karadioGet();
    uint16_t nameLen;

    if (clear || (krData->flags & KARADIO_FLAG_NAME)) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);
        nameLen = glcdWriteString(label);
        nameLen += glcdWriteString(" ");
        nameLen += glcdWriteString(krData->num);
        glcdDrawRect(canvas.glcd->x, canvas.glcd->y,
                     lt->rect.w - nameLen - iconSet->chars[0].image->width, lt->lblFont->chars[0].image->height,
                     canvas.pal->bg);
    }

    int16_t yPos = lt->lblFont->chars[0].image->height;

    if (clear || (krData->flags & KARADIO_FLAG_NAME)) {

        krData->flags &= ~KARADIO_FLAG_NAME;
        glcdSetFont(lt->rds.psFont);
        glcdSetXY(0, yPos);
        nameLen = glcdWriteString(krData->name);
        glcdDrawRect(canvas.glcd->x, canvas.glcd->y,
                     lt->rect.w - nameLen, lt->tuner.nameFont->chars[0].image->height,
                     canvas.pal->bg);
    }

    yPos += lt->rds.psFont->chars[0].image->height;

    if (clear || (krData->flags & KARADIO_FLAG_META)) {
        krData->flags &= ~KARADIO_FLAG_META;
        glcdSetFont(lt->rds.textFont);
        glcdSetXY(0, yPos);
        nameLen = glcdWriteString(krData->meta);
        glcdDrawRect(canvas.glcd->x, canvas.glcd->y,
                     lt->rect.w - nameLen, lt->rds.textFont->chars[0].image->height,
                     canvas.pal->bg);
    }

    yPos += lt->rds.textFont->chars[0].image->height;

    // Spectrum
    GlcdRect rect = canvas.glcd->rect;
    rect.y = yPos;
    rect.h = rect.h - rect.y;
    drawSpectrumMode(clear, rect);
}

void canvasShowAudioInput(bool clear, Icon icon)
{
    const Layout *lt = canvas.layout;

    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    if (!aProc->par.mute || ampGet()->status == AMP_STATUS_HW_READY) {
        switch (inType) {
        case IN_TUNER:
            if (NULL != tunerGet()->api) {
                canvasShowTuner(clear);
                return;
            }
            break;
        case IN_KARADIO:
            canvasShowKaradio(clear);
            return;
        case IN_BLUETOOTH:
        case IN_PC:
            break;
        default:
            if (NULL == aProc->api) {
                canvasShowSpectrum(clear);
                return;
            }
            break;
        }
    }

    const tFont *iconSet = lt->iconSet;
    const char *label = labelsGet(LABEL_IN_TUNER + inType);

    if (icon == ICON_EMPTY) {
        if (inType == IN_BLUETOOTH) {
            switch (btGetInput()) {
            case BT_IN_USB:
                icon = ICON_USB;
                break;
            case BT_IN_SDCARD:
                icon = ICON_CASSETTE;
                break;
            default:
                icon = ICON_BLUETOOTH;
                break;
            }
        } else {
            icon = (ICON_TUNER + inType);
        }
    }

    if (aProc->par.mute) {
        icon = ICON_MUTE_ON;
    }

    if (clear) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);
        glcdWriteString(label);
    }

    // Icon
    if (clear || icon != prev.par.icon) {
        prev.par.icon = icon;
        glcdSetXY(lt->rect.w - iconSet->chars[0].image->width, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas.pal->fg, canvas.pal->bg);
    }

    int16_t yPos = lt->lblFont->chars[0].image->height;

    BTCtx *btCtx = btCtxGet();

    if (inType == IN_BLUETOOTH) {
        if (clear || (btCtx->flags & BT_FLAG_NAME_CHANGED)) {
            btCtx->flags &= ~BT_FLAG_NAME_CHANGED;

            glcdSetFont(lt->rds.textFont);
            glcdSetXY(0, yPos);
            uint16_t nameLen = glcdWriteString(btGetSongName());
            glcdDrawRect(canvas.glcd->x, canvas.glcd->y,
                         lt->rect.w - nameLen, lt->rds.textFont->chars[0].image->height,
                         canvas.pal->bg);
        }
        yPos += lt->rds.textFont->chars[0].image->height;
    }

    // Spectrum
    GlcdRect rect = canvas.glcd->rect;
    rect.y = yPos;
    rect.h = rect.h - rect.y;
    drawSpectrumMode(clear, rect);
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

    int16_t yPos = lt->time.hmsY + lt->time.hmsFont->chars[0].image->height;

    // Spectrum
    GlcdRect rect = canvas.glcd->rect;
    rect.y = yPos;
    rect.h = rect.h - rect.y;
    drawSpectrumMode(clear, rect);
}
