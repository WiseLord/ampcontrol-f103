#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>
#include "eemap.h"

typedef uint16_t MuteStby;
enum {
    MUTESTBY_SWD = 0,
    MUTESTBY_POS,
    MUTESTBY_NEG,

    MUTESTBY_END,
};

void settingsInit(void);
int16_t settingsGet(EE_Param param);
void settingsSet(EE_Param param, int16_t value);
void settingsStore(EE_Param param);

#endif // SETTINGS_H
