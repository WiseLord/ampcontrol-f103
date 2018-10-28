#ifndef MENU_H
#define MENU_H

#include <stdint.h>

#define MENU_MAX_LEN    16

typedef enum {
    MENU_NULL = 0,

    MENU_SETUP,
    MENU_SETUP_AUDIO,
    MENU_SETUP_TUNER,
    MENU_SETUP_SPECTRUM,
    MENU_SETUP_DISPLAY,
    MENU_SETUP_INPUT,

    MENU_AUDIO_IC,

    MENU_TUNER_IC,
    MENU_TUNER_BAND,
    MENU_TUNER_STEP,
    MENU_TUNER_DEEMPH,
    MENU_TUNER_FMONO,
    MENU_TUNER_RDS,
    MENU_TUNER_BASS,
    MENU_TUNER_VOLUME,

    MENU_SPECTURM_MODE,
    MENU_SPECTRUM_SPEED,

    MENU_DISPLAY_BR_STBY,

    MENU_INPUT_ENC_RES,

    MENU_END
} MenuIdx;

typedef enum {
    MENU_TYPE_PARENT,
    MENU_TYPE_BOOL,
    MENU_TYPE_NUMBER,
    MENU_TYPE_ENUM,

    MENU_TYPE_END
} MenuType;

typedef struct {
    MenuIdx parent;
    MenuIdx active;
    uint8_t list[MENU_MAX_LEN];
    uint8_t listSize;       // Total number of items in current menu
    uint8_t selected;
    int16_t value;

    uint8_t dispSize;       // Number of items display able to show
    int8_t dispOft;         // First visible item offset on display
} Menu;

Menu *menuGet(void);

void menuSetActive(MenuIdx index);
void menuChange(int8_t diff);

MenuIdx menuGetFirstChild(void);

char *menuGetName(MenuIdx index);
char *menuGetValueStr(MenuIdx index);

#endif // MENU_H