#include "textedit.h"

#include <string.h>

void texteditSet(TextEdit *te, char *text, uint8_t maxLen, uint8_t maxSymbols)
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

void texteditChange(TextEdit *te, int8_t value)
{
    te->sPos += value;

    if (te->sPos < 0)
        te->sPos = 0;
    if (te->sPos > te->lastPos) {
        te->sPos = te->lastPos;
    }
}

void texteditAddChar(TextEdit *te)
{
    if (te->uLen >= te->maxSymbols) {
        return;
    }

    te->uStr[te->uLen] = glcdFontSymbolCode(te->sPos);
    te->uLen++;
    te->uStr[te->uLen] = 0;

    glcdUStrToStr(te->uStr, te->str);
    if ((int16_t)strlen(te->str) >= te->maxLen) {
        texteditDelChar(te);
    }
}

void texteditDelChar(TextEdit *te)
{
    if (te->uLen > 0) {
        te->uLen--;
    }
    te->uStr[te->uLen] = 0;

    glcdUStrToStr(te->uStr, te->str);
}
