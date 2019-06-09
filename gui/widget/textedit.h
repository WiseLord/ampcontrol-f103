#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../../display/glcd.h"

#define TE_STR_LEN      64
#define TE_USTR_LEN     32

typedef struct {
    char str[TE_STR_LEN];       // String to edit
    UChar uStr[TE_USTR_LEN];    // String being edited
    uint16_t uLen;              // Length of uStr
    int16_t sPos;               // Current symbol position
    int16_t lastPos;            // Last char position
    uint8_t maxLen;             // Maximum string length allowed
    uint8_t maxSymbols;         // Maximum symbols count allowed
} TextEdit;

void texteditSet(TextEdit *te, char *text, uint8_t maxLen, uint8_t maxSymbols);
void texteditChange(TextEdit *te, int8_t value);
void texteditAddChar(TextEdit *te);
void texteditDelChar(TextEdit *te);

#ifdef __cplusplus
}
#endif

#endif // TEXTEDIT_H
