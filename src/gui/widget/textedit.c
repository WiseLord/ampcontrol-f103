#include "textedit.h"
#include "../canvas.h"

#include <string.h>

void textEditSet(TextEdit *te, char *text, uint8_t maxLen, uint8_t maxSymbols)
{
    strncpy(te->str, text, maxLen);
    te->uLen = glcdStrToUStr(te->str, te->uStr);

    // Set active char to edit
    UChar uChar = 'R';
    if (te->uLen) {
        uChar = te->uStr[te->uLen - 1];
    }
    te->sPos = glcdFontSymbolPos(uChar);
    te->lastPos = glcdFontSymbolPos(LETTER_SPACE_CHAR) - 1;
    te->maxLen = maxLen;
    te->maxSymbols = maxSymbols;
}

void textEditChange(TextEdit *te, int8_t value)
{
    te->sPos += value;

    if (te->sPos < 0)
        te->sPos = 0;
    if (te->sPos > te->lastPos) {
        te->sPos = te->lastPos;
    }
}

void textEditAddChar(TextEdit *te)
{
    if (te->uLen >= te->maxSymbols) {
        return;
    }

    te->uStr[te->uLen] = glcdFontSymbolCode(te->sPos);
    te->uLen++;
    te->uStr[te->uLen] = 0;

    glcdUStrToStr(te->uStr, te->str);
    if ((int16_t)strlen(te->str) >= te->maxLen) {
        textEditDelChar(te);
    }
}

void textEditDelChar(TextEdit *te)
{
    if (te->uLen > 0) {
        te->uLen--;
    }
    te->uStr[te->uLen] = 0;

    glcdUStrToStr(te->uStr, te->str);
}

void textEditDraw(TextEdit *te, LayoutTextEdit *lt, bool clear)
{
    Canvas *canvas = canvasGet();

    Glcd *glcd = canvas->glcd;
    const Palette *pal = canvas->pal;

    const tFont *editFont = lt->editFont;
    const int16_t feh = editFont->chars[0].image->height;
    const int16_t few = editFont->chars[0].image->width;

    const GlcdRect *rect = &lt->rect;

    const int16_t yPos = (rect->h - feh) / 2;
    const int16_t xRoll = rect->w - few * 3 / 2;

    glcdSetFont(editFont);

    if (clear) {
        glcdSetXY(0, 0);
        glcdSetFontBgColor(pal->selected);
        glcdSetStringFramed(true);
        glcdWriteString(te->name);
        glcdSetStringFramed(false);
        // The rest of space after edit line
        glcdDrawRect(glcd->x, yPos, xRoll - glcd->x, feh, pal->selected);
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
    glcdDrawRect(xRoll - few / 4, 0, few / 4, rect->h, pal->selected);
    // The roller
    for (int8_t i = -2; i <= 2; i++) {
        glcdSetXY(xRoll, yPos + i * feh);
        int16_t sPos = te->sPos + i;
        UChar uCode = ' ';
        if (sPos >= 0 && sPos <= te->lastPos) {
            uCode = editFont->chars[te->sPos + i].code;
        }

        glcdSetFontBgColor(pal->selected);
        if (i == 0) {
            glcdSetFontColor(pal->fg);
        } else {
            glcdSetFontColor(pal->inactive);
        }
        glcdWriteUChar(LETTER_SPACE_CHAR);
        glcdWriteUChar(uCode);
        glcdWriteUChar(LETTER_SPACE_CHAR);

        glcdSetFontColor(pal->fg);
        glcdSetFontBgColor(pal->bg);
        glcdDrawRect(glcd->x, glcd->y, rect->w - glcd->x, feh, pal->selected);
    }
}
