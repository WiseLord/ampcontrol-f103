#include "settings.h"

#include "eemap.h"

static uint8_t silenceTimer = 0;

void settingsInit(void)
{
    silenceTimer = (uint8_t)eeRead(EE_SILENCE_TIMER);
}

int16_t settingsGet(EE_Param param)
{
    int16_t ret = 0;

    switch (param) {
    case EE_SILENCE_TIMER:
        ret = silenceTimer;
        break;
    default:
        break;
    }

    return  ret;
}

void settingsSet(EE_Param param, int16_t value)
{
    switch (param) {
    case EE_SILENCE_TIMER:
        silenceTimer = (uint8_t)value;
        break;
    default:
        break;
    }
}

void settingsStore(EE_Param param)
{
    eeUpdate(param, settingsGet(param));
}
