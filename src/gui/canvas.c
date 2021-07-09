#include "canvas.h"

#include <stdio.h>
#include <string.h>

#include "amp.h"
#include "bt.h"
#include "mpc.h"
#include "menu.h"
#include "rtc.h"
#include "settings.h"
#include "spectrum.h"
#include "swtimers.h"
#include "tr/labels.h"
#include "tuner/rds/parser.h"
#include "tuner/stations.h"
#include "utils.h"
#include "view/starsview.h"
#include "widget/scrolltext.h"

#define SPECTRUM_SIZE   128

typedef union {
    RTC_type rtc;
    int16_t wtfX;
    struct {
        uint16_t freq;
        int16_t value;
        Icon icon;
        bool cross;
        bool stereo;
        bool forcedMono;
        bool rdsFlag;
    } par;
    int timer;
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
static void calcSpCol(int16_t chan, int16_t scale, uint8_t col, SpectrumColumn *spCol,
                      SpData *spData);
static void drawWaterfall(bool clear);
static void drawSpectrum(bool clear, bool check, bool mirror, SpChan chan, GlcdRect *rect);
static void drawSpectrumMode(bool clear, GlcdRect rect);
static void drawRds(RdsParser *rds);
static bool checkSpectrumReady(void);
static void fftGet128(FftSample *sp, uint8_t *out, size_t size);

static Canvas canvas;
static SpDrawData spDrawData;
static DrawData prev;
static ScrollText scroll;

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

static void drawCrosssing(int16_t x, int16_t y, int16_t w)
{
    const Palette *pal = paletteGet();

    int8_t th = w / 20 + 1;

    for (int8_t i = 0; i <= th; i++) {
        color_t color = (i >= th ? pal->bg : pal->fg);
        glcdDrawLine(x + i, y, x + w - 1, y + w - 1 - i, color);
        glcdDrawLine(x, y + i, x + w - 1 - i, y + w - 1, color);

        glcdDrawLine(x + i, y + w - 1, x + w - 1, y + i, color);
        glcdDrawLine(x, y + w - 1 - i, x + w - 1 - i, y, color);
    }
}

void canvasInit(void)
{
    bool rotate = settingsRead(PARAM_DISPLAY_ROTATE, false);
    PalIdx palIdx = (PalIdx)settingsRead(PARAM_DISPLAY_PALETTE, PAL_DEFAULT);

    glcdInit(rotate ? GLCD_LANDSCAPE_ROT : GLCD_LANDSCAPE);

    canvas.glcd = glcdGet();
    canvas.layout = layoutGet();

    paletteSetIndex(palIdx);
    canvas.pal = paletteGet();

    glcdDrawRect(0, 0, canvas.layout->rect.w, canvas.layout->rect.h, canvas.pal->bg);

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
        char buf[8] = "--";
        if (time >= 0) {
            snprintf(buf, sizeof(buf), "%02d", time);
        }
        glcdWriteString(buf);
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

    char buf[64];
    menuGetName(menuIdx, buf, sizeof(buf));
    glcdWriteString(buf);

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
    menuGetValueStr(menuIdx, buf, sizeof(buf));
    uint16_t strLen = glcdWriteString(buf);
    glcdSetStringFramed(false);

    glcdSetFontColor(color);
    glcdSetFontBgColor(bgColor);

    // Fill space between name and value
    glcdDrawRect(x, y_pos + 2, width - 2 - x - strLen, fIh, canvas.pal->bg);
}

static void calcSpCol(int16_t chan, int16_t scale, uint8_t col, SpectrumColumn *spCol,
                      SpData *spData)
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

    SpData spData[SP_CHAN_END];

    spGetADC(SP_CHAN_LEFT, spData[SP_CHAN_LEFT].raw, SPECTRUM_SIZE, fftGet128);
    spGetADC(SP_CHAN_RIGHT, spData[SP_CHAN_RIGHT].raw, SPECTRUM_SIZE, fftGet128);

    const Layout *lt = canvas.layout;

    if (clear || ++prev.wtfX >= lt->rect.w) {
        prev.wtfX = 0;
    }
    glcdShift((prev.wtfX + 1) % lt->rect.w);

    for (uint8_t col = 0; col < SPECTRUM_SIZE; col++) {
        SpectrumColumn spCol;
        calcSpCol(SP_CHAN_BOTH, 224, col, &spCol, spData);
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
        if (sp->flags & SP_FLAG_GRAD) {
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
        swTimSet(SW_TIM_SP_CONVERT, 25);
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

        db = spGetDb(calc);

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

    SpData spData[SP_CHAN_END];

    if (clear) {
        memset(&spDrawData, 0, sizeof (SpDrawData));
        memset(spData, 0, sizeof(spData));
    } else {
        if (chan == SP_CHAN_LEFT || chan == SP_CHAN_BOTH) {
            spGetADC(SP_CHAN_LEFT, spData[SP_CHAN_LEFT].raw, SPECTRUM_SIZE, fftGet128);
        }
        if (chan == SP_CHAN_RIGHT || chan == SP_CHAN_BOTH) {
            spGetADC(SP_CHAN_RIGHT, spData[SP_CHAN_RIGHT].raw, SPECTRUM_SIZE, fftGet128);
        }
    }

    const int16_t step = (rect->w  + 1) / SPECTRUM_SIZE + 1;    // Step of columns
    const int16_t colW = step - (step / 2);                     // Column width
    const int16_t num = (rect->w + colW - 1) / step;            // Number of columns

    const int16_t width = (num - 1) * step + colW;              // Width of spectrum
    const int16_t height = rect->h;                             // Height of spectrum

    const int16_t oft = (rect->w - width) / 2;                  // Spectrum offset for symmetry

    const int16_t y = rect->y;

    Spectrum *sp = spGet();

    color_t grad[512];

    if (sp->flags & SP_FLAG_GRAD) {
        calcGradient(sp, height, mirror, grad);
    }

    for (uint8_t col = 0; col < num; col++) {
        int16_t x = oft + col * step;

        SpectrumColumn spCol;
        calcSpCol(chan, height, col, &spCol, spData);
        if (!(sp->flags & SP_FLAG_PEAKS)) {
            spCol.peakW = 0;
        }
        GlcdRect rect = {x, y, colW, height};
        spectrumColumnDraw(clear, &spCol, &rect, mirror, grad);
    }
}

static void drawRds(RdsParser *rds)
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
    const Layout *lt = canvas.layout;

    Menu *menu = menuGet();

    const int16_t fHh = (int16_t)lt->menu.headFont->chars[0].image->height;
    const int16_t fIh = (int16_t)lt->menu.menuFont->chars[0].image->height;
    const uint8_t items = menu->dispSize;

    const int16_t dividerPos = (lt->rect.h - (fIh + 4) * items + fHh) / 2;

    // Show header
    glcdSetFont(lt->menu.headFont);
    glcdSetFontColor(canvas.pal->fg);

    static uint8_t lastActive = 0;
    static uint8_t lastSelected = 0;
    static uint8_t lastValue = 0;

    if (menu->active != lastActive ||
        menu->selected != lastSelected ||
        menu->value != lastValue) {
        clear = true;
    }

    if (clear) {
        glcdSetXY(2, 0);

        char buf[64];
        menuGetName(menu->parent, buf, sizeof(buf));
        glcdWriteString(buf);

        // Fill free space after header
        glcdDrawRect(canvas.glcd->x, canvas.glcd->y, lt->rect.w - canvas.glcd->x, fHh, canvas.pal->bg);
        // Line between header and menu items
        glcdDrawRect(0, dividerPos, lt->rect.w, 1, canvas.glcd->fontFg);
    }

    for (uint8_t idx = 0; idx < menu->listSize; idx++) {
        if (idx >= menu->dispOft && idx < items + menu->dispOft) {
            if (clear) {
                drawMenuItem(idx, lt->menu.menuFont);
            }
        }
    }

    lastActive = menu->active;
    lastSelected = menu->selected;
    lastValue = menu->value;
}

void canvasShowTune(bool clear, AudioTune tune)
{
    AudioProc *aProc = audioGet();
    Amp *amp = ampGet();

    if (NULL == aProc->api) {
        canvasShowSpectrum(clear);
        return;
    }

    const Layout *lt = canvas.layout;
    const tFont *iconSet = lt->iconSet;

    InputType inType = amp->inType[aProc->par.input];

    bool cross = false;

    const char *label = labelsGet(inType == IN_DISABLED ? LABEL_BOOL_OFF : LABEL_IN_TUNER + inType);
    Icon icon = ICON_TUNER + inType;
    if (inType == IN_DISABLED) {
        icon = ICON_VOLUME;
        cross = true;
    }

    if (tune < AUDIO_TUNE_GAIN) {
        label = labelsGet(LABEL_VOLUME + tune);
        icon = ICON_VOLUME + tune;
        if (tune > AUDIO_TUNE_LOUDNESS) {
            icon = ICON_EMPTY; // TODO icons
        }
    }

    if (icon == ICON_VOLUME && (aProc->par.flags & AUDIO_FLAG_MUTE)) {
        cross = true;
    }

    const AudioGrid *grid = aProc->par.grid[tune];
    const int16_t value = aProc->par.tune[tune];

    const int8_t min = grid ? grid->min : 0;
    const int8_t max = grid ? grid->max : 0;
    const int8_t mStep = grid ? grid->mStep : 0;

    glcdSetFont(lt->lblFont);
    glcdSetFontColor(canvas.pal->fg);

    if (clear) {
        // Label
        glcdSetXY(0, 0);
        glcdWriteString(label);
    }

    if ((clear || icon != prev.par.icon || cross != prev.par.cross) && icon != ICON_EMPTY) {
        // Icon
        int16_t w = iconSet->chars[0].image->width;
        int16_t x = lt->rect.w - w;
        glcdSetXY(x, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas.pal->fg, canvas.pal->bg);
        if (cross) {
            drawCrosssing(x, 0, w);
        }
        prev.par.icon = icon;
        prev.par.cross = cross;
    }

    if (clear || value != prev.par.value) {
        // Value
        glcdSetXY(lt->rect.w, lt->tune.valY);
        glcdSetFontAlign(GLCD_ALIGN_RIGHT);
        glcdSetFont(lt->tune.valFont);

        int16_t showValue = value;

        char bufValue[16] = "";

        if (grid->array != NULL) {
            showValue = grid->array[value];
        }

        switch (tune) {
        case AUDIO_TUNE_BASS_FREQ:
        case AUDIO_TUNE_SUB_CUT_FREQ:
        case AUDIO_TUNE_LOUD_PEAK_FREQ:
            snprintf(bufValue, sizeof(bufValue), "%4d%s", showValue / STEP_MULT, labelsGet(LABEL_HZ));
            break;
        case AUDIO_TUNE_MIDDLE_KFREQ:
        case AUDIO_TUNE_TREBLE_KFREQ:
            showValue = showValue * 1000 / STEP_MULT;
            snprintf(bufValue, sizeof(bufValue), "%3d.%1d%s%s", showValue / 1000, showValue % 1000 / 100,
                     labelsGet(LABEL_K), labelsGet(LABEL_HZ));
            break;
        case AUDIO_TUNE_BASS_QUAL:
        case AUDIO_TUNE_MIDDLE_QUAL:
            showValue = showValue * 1000 / STEP_MULT;
            snprintf(bufValue, sizeof(bufValue), "%2d.%02d", showValue / 1000, showValue % 1000 / 10);
            break;
        default:
            if (amp->showDb) {
                showValue = value * mStep / STEP_MULT;
            } else {
                if (min + max != 0) {
                    showValue -= min;
                }
            }
            snprintf(bufValue, sizeof(bufValue), "%3d", showValue);
        }

        int16_t strLen = glcdWriteString(bufValue);


        // Bar
        StripedBar bar = {value, min, max};
        LayoutStripedBar ltBar = lt->tune.bar;

        ltBar.barW = lt->rect.w - strLen;

        stripedBarDraw(clear, &bar, &ltBar);

        prev.par.value = value;
    }

    int16_t yPos = lt->tune.valY + lt->tune.labelFont->chars[0].image->height;

    GlcdRect rect = canvas.glcd->rect;
    rect.y = yPos;
    rect.h = rect.h - rect.y;
    drawSpectrumMode(clear, rect);
}

void canvasShowAudioFlag(bool clear, AudioTune flag)
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
    bool cross = false;

    switch (flag) {
    case AUDIO_FLAG_LOUDNESS:
        label = (Label)(LABEL_MENU + MENU_RC_LOUDNESS);
        icon = ICON_LOUDNESS;
        if (!(aProc->par.flags & AUDIO_FLAG_LOUDNESS)) {
            cross = true;
        }
        break;
    case AUDIO_FLAG_SURROUND:
        label = (Label)(LABEL_MENU + MENU_RC_SURROUND);
        icon = ICON_SURROUND;
        if (!(aProc->par.flags & AUDIO_FLAG_SURROUND)) {
            cross = true;
        }
        break;
    case AUDIO_FLAG_EFFECT3D:
        label = (Label)(LABEL_MENU + MENU_RC_EFFECT_3D);
        icon = ICON_EFFECT_3D;
        if (!(aProc->par.flags & AUDIO_FLAG_EFFECT3D)) {
            cross = true;
        }
        break;
    case AUDIO_FLAG_BYPASS:
        label = (Label)(LABEL_MENU + MENU_RC_TONE_BYPASS);
        icon = ICON_TONE;
        if (aProc->par.flags & AUDIO_FLAG_BYPASS) {
            cross = true;
        }
        break;
    default:
        label = (Label)(LABEL_MENU + MENU_RC_MUTE);
        icon = ICON_VOLUME;
        if (aProc->par.flags & AUDIO_FLAG_MUTE) {
            cross = true;
        }
        break;
    }

    glcdSetFont(lt->lblFont);
    glcdSetFontColor(canvas.pal->fg);

    if (clear) {
        // Label
        glcdSetXY(0, 0);
        glcdWriteString(labelsGet(label));
    }

    // Icon
    if (clear || icon != prev.par.icon || cross != prev.par.cross) {
        int16_t w = iconSet->chars[0].image->width;
        int16_t x = lt->rect.w - w;
        glcdSetXY(x, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas.pal->fg, canvas.pal->bg);
        if (cross) {
            drawCrosssing(x, 0, w);
        }
        prev.par.icon = icon;
        prev.par.cross = cross;
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
    case SP_MODE_INVERTED:
    case SP_MODE_ANTIMIRROR:
        rect.h = rect.h / 2;
        drawSpectrum(clear, true,
                     sp->mode == SP_MODE_ANTIMIRROR || sp->mode == SP_MODE_INVERTED,
                     SP_CHAN_LEFT, &rect);
        rect.y += rect.h;
        drawSpectrum(clear, false,
                     sp->mode == SP_MODE_MIRROR || sp->mode == SP_MODE_INVERTED,
                     SP_CHAN_RIGHT, &rect);
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
    case SP_MODE_INVERTED:
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

void canvasShowStars(bool clear, int16_t offset)
{
    StarsView view;
    view.offset = offset;
    view.stars = &spDrawData;

    if (swTimGet(SW_TIM_SP_CONVERT) == 0) {
        swTimSet(SW_TIM_SP_CONVERT, 10);
        starsView(&view, clear);
    }
}

void canvasShowTuner(bool clear)
{
    const Layout *lt = canvas.layout;

    Tuner *tuner = tunerGet();
    RdsParser *rdsParser = rdsParserGet();

    const tFont *iconSet = lt->iconSet;

    const tImage *icon = NULL;
    const uint8_t iconSpace = lt->tuner.iconSpace;
    LayoutStripedBar ltTunerBar = lt->tuner.bar;

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

        char buf[10];
        snprintf(buf, sizeof(buf), "FM %3d.%02d", freq / 100, freq % 100);
        glcdWriteString(buf);

        // Scale
        StripedBar bar = {(int16_t)freq, freqMin, freqMax};
        stripedBarDraw(clear, &bar, &ltTunerBar);

        glcdSetFont(lt->tuner.stFont);
        glcdSetXY(lt->rect.w, lt->tune.valY);
        glcdSetFontAlign(GLCD_ALIGN_RIGHT);

        // Station number
        int8_t stNum = stationGetNum(freq);
        if (stNum >= 0) {
            snprintf(buf, sizeof(buf), "%02d", stNum);
        } else {
            snprintf(buf, sizeof(buf), "--");
        }
        glcdWriteString(buf);

        // Station name
        glcdSetFont(lt->tuner.nameFont);

        int16_t fh = glcdGetFontHeight(lt->tuner.nameFont);
        glcdSetXY(0, lt->rect.h / 2 - fh);

        uint16_t nameLen = glcdWriteString(stationGetName(stNum));
        glcdDrawRect(canvas.glcd->x, canvas.glcd->y, lt->tuner.bar.barW - nameLen, fh, canvas.pal->bg);
    }

    // Stereo / forced mono indicator
    bool forcedMono = ((tuner->par.flags & TUNER_PARAM_MONO) == TUNER_PARAM_MONO);
    bool stereo = ((tuner->status.flags & TUNER_STATUS_STEREO) == TUNER_STATUS_STEREO);

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
    bool rdsFlag = ((rdsParser->flags & RDS_FLAG_READY) == RDS_FLAG_READY);

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
        drawRds(rdsParser);
        return;
    }

    rdsParserReset();

    // Spectrum
    GlcdRect rect = canvas.glcd->rect;
    rect.h /= 2;
    rect.y = rect.h;
    drawSpectrumMode(clear, rect);
}

static void scrollMpdMeta(bool clear, int16_t yPos, const char *inName)
{
    const Layout *lt = canvas.layout;

    Mpc *mpc = mpcGet();

    if (mpc->flags & (MPC_FLAG_UPDATE_META | MPC_FLAG_UPDATE_STATUS)) {
        clear = true;
    }

    glcdSetFont(lt->rds.textFont);

    static GlcdRect rectMeta;
    rectMeta.x = 0;
    rectMeta.y = yPos;
    rectMeta.w = lt->rect.w;
    rectMeta.h = lt->rds.textFont->chars[0].image->height;

    scroll.rect = &rectMeta;

    if (swTimGet(SW_TIM_SCROLL) <= 0) {
        swTimSet(SW_TIM_SCROLL, 50);
        scroll.flags |= SCROLL_EVENT;
    } else {
        scroll.flags &= ~SCROLL_EVENT;
    }

    char buf[32];

    if (mpc->status == MPC_IDLE) {
        snprintf(buf, sizeof(buf), "%s %s", labelsGet(LABEL_IN_WAIT), inName);
        scroll.text = buf;
    } else if (mpc->status & MPC_PLAYING) {
        scroll.text = mpc->meta;
    } else {
        scroll.text = mpc->ip;
    }

    scrollTextDraw(&scroll, clear);
}

void canvasShowMpd(bool clear, Icon icon)
{
    (void)icon;

    const Layout *lt = canvas.layout;

    const Palette *pal = paletteGet();

    const tFont *iconSet = lt->iconSet;
    const char *label = labelsGet(LABEL_IN_MPD);

    Mpc *mpc = mpcGet();
    uint16_t nameLen;
    char buf[32];

    glcdSetFontColor(pal->fg);

    // Label + number
    if (clear || (mpc->flags & MPC_FLAG_UPDATE_TRACKNUM)) {
        glcdSetFont(lt->lblFont);
        glcdSetXY(0, 0);
        nameLen = glcdWriteString(label);
        nameLen += glcdWriteString(" ");
        if (mpc->trackNum >= 0) {
            snprintf(buf, sizeof(buf), "%-3d", (int)mpc->trackNum);
        } else {
            snprintf(buf, sizeof (buf), "%s", "   ");
        }
        nameLen += glcdWriteString(buf);
    }

    // MPD icons
    if (clear || (mpc->flags & MPC_FLAG_UPDATE_STATUS)) {
        const tImage *img;

        img = glcdFindIcon(ICON_REPEAT, iconSet);
        glcdSetXY(lt->rect.w - img->width * 11 / 2, 0);
        glcdDrawImage(img, mpc->status & MPC_REPEAT ? pal->fg : pal->selected, pal->bg);

        img = glcdFindIcon(ICON_SINGLE, iconSet);
        glcdSetXY(lt->rect.w - img->width * 8 / 2, 0);
        glcdDrawImage(img, mpc->status & MPC_SINGLE ? pal->fg : pal->selected, pal->bg);

        img = glcdFindIcon(ICON_RANDOM, iconSet);
        glcdSetXY(lt->rect.w - img->width * 5 / 2, 0);
        glcdDrawImage(img, mpc->status & MPC_RANDOM ? pal->fg : pal->selected, pal->bg);

        img = glcdFindIcon(ICON_CONSUME, iconSet);
        glcdSetXY(lt->rect.w - img->width * 2 / 2, 0);
        glcdDrawImage(img, mpc->status & MPC_CONSUME ? pal->fg : pal->selected, pal->bg);
    }

    int16_t yPos = glcdFindIcon(ICON_REPEAT, iconSet)->height;

    int time = mpc->elapsed;

    int8_t sec = time % 60;
    time /= 60;
    int8_t min = time % 60;
    time /= 60;
    int8_t hour = time % 24;
    time /= 24;

    if (!(mpc->status & MPC_PLAYING)) {
        snprintf(buf, sizeof(buf), "--:--:--");
    } else if (time > 0) {
        snprintf(buf, sizeof(buf), "%02d.%02d:%02d", time, hour, min);
    } else {
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour, min, sec);
    }
    char *pos = buf;

    // Position timer
    if (clear || (mpc->flags & (MPC_FLAG_UPDATE_NAME | MPC_FLAG_UPDATE_ELAPSED))) {
        glcdSetFont(lt->rds.psFont);
        glcdSetFontAlign(GLCD_ALIGN_RIGHT);
        glcdSetXY(lt->rect.w, yPos);
        nameLen = glcdWriteString(pos);
    }

    yPos += lt->rds.psFont->chars[0].image->height;

    // Meta
    scrollMpdMeta(clear, yPos, label);

    yPos += lt->rds.textFont->chars[0].image->height;

    // Spectrum
    GlcdRect rect = canvas.glcd->rect;
    rect.y = yPos;
    rect.h = rect.h - rect.y;
    drawSpectrumMode(clear, rect);

    mpc->flags = 0;
}

void canvasShowKaradio(bool clear, Icon icon)
{
    const Layout *lt = canvas.layout;
    const Palette *pal = paletteGet();

    const tFont *iconSet = lt->iconSet;
    const char *label = labelsGet(LABEL_IN_KARADIO);

    if (icon == ICON_EMPTY) {
        icon = ICON_KARADIO;
    }

    if (clear || icon != prev.par.icon) {
        // Icon
        glcdSetXY(lt->rect.w - iconSet->chars[0].image->width, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, pal->fg, pal->bg);
    }

    Mpc *mpc = mpcGet();
    uint16_t nameLen;
    char buf[32];

    glcdSetFontColor(pal->fg);

    // Label + number
    if (clear || (mpc->flags & MPC_FLAG_UPDATE_TRACKNUM)) {
        glcdSetFont(lt->lblFont);
        glcdSetXY(0, 0);
        nameLen = glcdWriteString(label);
        nameLen += glcdWriteString(" ");
        if (mpc->trackNum >= 0) {
            snprintf(buf, sizeof(buf), "%ld", mpc->trackNum);
            nameLen += glcdWriteString(buf);
        }
        glcdDrawRect(canvas.glcd->x, canvas.glcd->y,
                     lt->rect.w - nameLen - iconSet->chars[0].image->width, lt->lblFont->chars[0].image->height,
                     pal->bg);
    }

    int16_t yPos = lt->lblFont->chars[0].image->height;

    char *name = mpc->name;

    // Name
    if (clear || (mpc->flags & (MPC_FLAG_UPDATE_NAME | MPC_FLAG_UPDATE_ELAPSED))) {
        glcdSetFont(lt->rds.psFont);
        glcdSetXY(0, yPos);
        nameLen = glcdWriteString(name);
        glcdDrawRect(canvas.glcd->x, canvas.glcd->y,
                     lt->rect.w - nameLen, lt->tuner.nameFont->chars[0].image->height, pal->bg);
    }

    yPos += lt->rds.psFont->chars[0].image->height;

    // Meta
    scrollMpdMeta(clear, yPos, label);

    yPos += lt->rds.textFont->chars[0].image->height;

    // Spectrum
    GlcdRect rect = canvas.glcd->rect;
    rect.y = yPos;
    rect.h = rect.h - rect.y;
    drawSpectrumMode(clear, rect);

    mpc->flags = 0;
}

void canvasShowAudioInput(bool clear, Icon icon)
{
    const Layout *lt = canvas.layout;

    AudioProc *aProc = audioGet();
    InputType inType = ampGet()->inType[aProc->par.input];

    if (!(aProc->par.flags & AUDIO_FLAG_MUTE) || ampGet()->status == AMP_STATUS_HW_READY) {
        switch (inType) {
        case IN_TUNER:
            if (NULL != tunerGet()->api) {
                canvasShowTuner(clear);
                return;
            }
            break;
        case IN_MPD:
            canvasShowMpd(clear, icon);
            return;
        case IN_KARADIO:
            canvasShowKaradio(clear, icon);
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
    const char *label = labelsGet(inType == IN_DISABLED ? LABEL_BOOL_OFF : LABEL_IN_TUNER + inType);

    if (inType == IN_BLUETOOTH) {
        switch (btGetInput()) {
        case BT_IN_USB:
            label = labelsGet(LABEL_IN_USB);
            break;
        case BT_IN_SDCARD:
            label = labelsGet(LABEL_IN_SDCARD);
            break;
        default:
            label = labelsGet(LABEL_IN_BLUETOOTH);
            break;
        }
    }

    bool cross = false;

    if (icon == ICON_EMPTY) {
        if (inType == IN_BLUETOOTH) {
            switch (btGetInput()) {
            case BT_IN_USB:
                icon = ICON_USB;
                break;
            case BT_IN_SDCARD:
                icon = ICON_SDCARD;
                break;
            default:
                icon = ICON_BLUETOOTH;
                break;
            }
        } else {
            icon = ICON_TUNER + inType;
            if (inType == IN_DISABLED) {
                icon = ICON_VOLUME;
                cross = true;
            }
        }
    }

    if (aProc->par.flags & AUDIO_FLAG_MUTE) {
        icon = ICON_VOLUME;
        cross = true;
    }

    if (clear || icon != prev.par.icon || cross != prev.par.cross) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);
        int16_t nameLen = glcdWriteString(label);
        int16_t w = iconSet->chars[0].image->width;
        int16_t x = lt->rect.w - w;

        glcdDrawRect(canvas.glcd->x, canvas.glcd->y, x - nameLen,
                     lt->lblFont->chars[0].image->height, canvas.pal->bg);

        glcdSetXY(x, 0);
        const tImage *img = glcdFindIcon(icon, iconSet);
        glcdDrawImage(img, canvas.pal->fg, canvas.pal->bg);
        if (cross) {
            drawCrosssing(x, 0, w);
        }
        prev.par.icon = icon;
        prev.par.cross = cross;
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

    textEditDraw(clear, te, &lt->textEdit);
}

void canvasShowTimer(bool clear, int32_t value)
{
    const Layout *lt = canvas.layout;

    char buf[32];

    int timer = value / 1000;

    if (clear || timer != prev.timer) {
        prev.timer = timer;
        if (timer <= 0) {
            snprintf(buf, sizeof(buf), "%s", "--:--:--");
        } else {
            snprintf(buf, sizeof(buf), "%02d:%02d:%02d", timer / 3600, timer / 60 % 60, timer % 60);
        }

        // HH:MM:SS
        glcdSetFont(lt->time.hmsFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetFontBgColor(canvas.pal->bg);
        glcdSetFontAlign(GLCD_ALIGN_CENTER);
        glcdSetXY(lt->rect.w / 2, lt->time.hmsY);

        glcdWriteString(buf);
    }

    int16_t yPos = lt->time.hmsY + lt->time.hmsFont->chars[0].image->height;

    // Spectrum
    GlcdRect rect = canvas.glcd->rect;
    rect.y = yPos;
    rect.h = rect.h - rect.y;
    drawSpectrumMode(clear, rect);
}

void canvasDebugFPS(void)
{
    return;

    const Palette *pal = canvas.pal;
    const Layout *lt = canvas.layout;

    glcdSetFont(lt->menu.menuFont);
    glcdSetFontColor(pal->active);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);

    static int32_t oldCnt = 0;
    static int32_t oldFps = 0;
    (void)oldFps;
    static int32_t frames = 0;

    frames++;

    int32_t cnt = swTimGet(SW_TIM_SYSTEM);
    if (cnt - oldCnt > 500) {
        int32_t fps = frames * 1000 / (cnt - oldCnt);
        frames = 0;
        oldFps = fps;
        oldCnt = cnt;
    } else {
    }

    char buf[16];

    glcdSetXY(canvas.glcd->rect.w, canvas.glcd->rect.h - lt->menu.menuFont->chars[0].image->height);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    snprintf(buf, sizeof(buf), "%03d", (int)oldFps);
    glcdWriteString(buf);
}

void canvasDebugTimers()
{
    return;

    const Palette *pal = canvas.pal;
    const Layout *lt = canvas.layout;
    const tFont *font = lt->menu.menuFont;

    const int16_t stepY = font->chars[0].image->height;
    const int16_t stepX = font->chars[0].image->width * 12;

    glcdSetFont(lt->menu.menuFont);
    glcdSetFontColor(pal->active);

    char buf[16];

    for (uint8_t i = 0; i < SW_TIM_END; i++) {
        glcdSetXY((i / 10) * stepX, (i % 10) * stepY - 1);
        snprintf(buf, sizeof(buf), "%10d", (int)swTimGet(i));
        glcdWriteString(buf);
    }
}
