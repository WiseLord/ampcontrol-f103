#include "canvas.h"

Canvas *canvas;

void canvasInit(Canvas **value)
{
#if defined (_KS0108A) || defined(_KS0108B) || defined (_ST7920) || defined (_SSD1306)
    gm128x64Init(value);
#elif defined (_ILI9163) || defined (_ST7735)
    gc160x128Init(value);
#elif defined (_LS020) || defined (_LPH9157) || defined (_SSD1286A)
    gc176x132Init(value);
#elif defined (_HX8340) || defined (_ILI9225)
    gc220x176Init(value);
#elif defined (_ILI9320) || defined (_ILI9341) || defined (_S6D0139) || defined (_SPFD5408) || defined (_MC2PA8201)
    gc320x240Init(value);
#elif defined (_ILI9327) || defined (_ST7793)
    gc400x240Init(value);
#elif defined (_ILI9481) || defined (_R61581)
    gc480x320Init(value);
#else
#ifdef EMUL_DISP
    emulCanvasInit(value);
#else
#error "Unsupported display driver"
#endif
#endif
    (*value)->color = LCD_COLOR_BLACK;

    canvas = *value;
}

static void canvasShowMenuItem(uint8_t idx, const tFont *fontItem)
{
    int16_t fIh = fontItem->chars[0].image->height;

    Menu *menu = menuGet();
    uint8_t items = menu->dispSize;

    uint16_t width = canvas->width;
    MenuIdx menuIdx = menu->list[idx];
    char *name = menuGetName(menuIdx);
    uint8_t active = (menu->active == menu->list[idx]);

    const uint8_t ih = fIh + 4; // Menu item height
    uint16_t y_pos = canvas->height - ih * (items - idx + menu->dispOft);

    // Draw selection frame
    glcdDrawFrame(0, y_pos, width - 1, y_pos + ih - 1, active ? LCD_COLOR_WHITE : canvas->color);

    // Draw menu name
    glcdSetFont(fontItem);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(4, y_pos + 2);
    if (menu->list[idx] != MENU_NULL) {
        glcdWriteString("  ");
    } else {
        glcdWriteString("< ");
    }
    glcdWriteString(name);

    // Draw menu value
    uint16_t x = canvas->glcd->x;
    glcdSetXY(width - 2, y_pos + 2);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);

    // Inverse value color if selected
    uint16_t color = canvas->glcd->font.color;
    uint16_t bgColor = canvas->glcd->font.bgColor;
    if (active && menu->selected) {
        glcdSetFontColor(bgColor);
        glcdSetFontBgColor(color);
    }
    uint16_t strLen = glcdWriteStringFramed(menuGetValueStr(menuIdx), 1);
    glcdSetFontColor(color);
    glcdSetFontBgColor(bgColor);

    // Fill space between name and value
    glcdDrawRect(x, y_pos + 2, width - 2 - x - strLen, fIh, canvas->color);
}

void canvasShowMenu(const tFont *fontHeader, const tFont *fontItem)
{

    Menu *menu = menuGet();

    int16_t fHh = fontHeader->chars[0].image->height;
    int16_t fIh = fontItem->chars[0].image->height;
    uint8_t items = menu->dispSize;

    int16_t dividerPos = (canvas->height - (fIh + 4) * items + fHh) / 2;

    // Show header
    char *parentName = menuGetName(menu->parent);
    glcdSetFont(fontHeader);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString(parentName);

    glcdDrawRect(0, dividerPos, canvas->width, 1, canvas->glcd->font.color);

    for (uint8_t idx = 0; idx < menu->listSize; idx++) {
        if (idx >= menu->dispOft && idx < items + menu->dispOft) {
            canvasShowMenuItem(idx, fontItem);
        }
    }
}
