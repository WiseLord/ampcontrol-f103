#ifndef MENU_H
#define MENU_H

#include <inttypes.h>

#define MENU_MAX_LEN    16

typedef enum {
    MENU_NULL = 0,

    MENU_SETUP,
    MENU_SETUP_AUDIO,
    MENU_SETUP_TUNER,
    MENU_SETUP_SPECTRUM,
    MENU_SETUP_DISPLAY,
    MENU_SETUP_INPUT,

    MENU_TUNER_IC,
    MENU_TUNER_FREQ_MIN,
    MENU_TUNER_FREQ_MAX,
    MENU_TUNER_STEP,
    MENU_TUNER_MONO,
    MENU_TUNER_RDS,
    MENU_TUNER_BASS,
    MENU_TUNER_BAND,
    MENU_TUNER_DE,
    MENU_TUNER_SOFTMUTE,

    MENU_END
} MenuIdx;

typedef struct {
    MenuIdx parent;
} MenuItem;

typedef struct {
    MenuIdx parent;
    MenuIdx active;
    uint8_t idx[MENU_MAX_LEN];
    uint8_t size;
    uint8_t oft;
} Menu;

void menuInit(void);
void menuSetActive(MenuIdx index);

Menu *menuGet(void);

MenuIdx menuGetParent(MenuIdx index);

char *menuGetName(MenuIdx index);

#endif // MENU_H
