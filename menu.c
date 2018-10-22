#include "menu.h"
#include "tr/labels.h"

static Menu menu;

typedef struct {
    MenuIdx parent;
    MenuType type;
} MenuItem;

static const MenuItem menuItems[MENU_END] = {
    //                          parent menu
    [MENU_NULL]             = { MENU_NULL,              MENU_TYPE_PARENT},

    [MENU_SETUP]            = { MENU_NULL,              MENU_TYPE_PARENT},

    [MENU_SETUP_AUDIO]      = { MENU_SETUP,             MENU_TYPE_PARENT},
    [MENU_SETUP_TUNER]      = { MENU_SETUP,             MENU_TYPE_PARENT},
    [MENU_SETUP_SPECTRUM]   = { MENU_SETUP,             MENU_TYPE_PARENT},
    [MENU_SETUP_DISPLAY]    = { MENU_SETUP,             MENU_TYPE_PARENT},
    [MENU_SETUP_INPUT]      = { MENU_SETUP,             MENU_TYPE_PARENT},

    [MENU_AUDIO_IC]         = { MENU_SETUP_AUDIO,       MENU_TYPE_ENUM},

    [MENU_TUNER_IC]         = { MENU_SETUP_TUNER,       MENU_TYPE_ENUM},
    [MENU_TUNER_FREQ_MIN]   = { MENU_SETUP_TUNER,       MENU_TYPE_NUMBER},
    [MENU_TUNER_FREQ_MAX]   = { MENU_SETUP_TUNER,       MENU_TYPE_NUMBER},
    [MENU_TUNER_STEP]       = { MENU_SETUP_TUNER,       MENU_TYPE_NUMBER},
    [MENU_TUNER_MONO]       = { MENU_SETUP_TUNER,       MENU_TYPE_BOOL},
    [MENU_TUNER_RDS]        = { MENU_SETUP_TUNER,       MENU_TYPE_BOOL},
    [MENU_TUNER_BASS]       = { MENU_SETUP_TUNER,       MENU_TYPE_BOOL},
    [MENU_TUNER_BAND]       = { MENU_SETUP_TUNER,       MENU_TYPE_ENUM},
    [MENU_TUNER_DE]         = { MENU_SETUP_TUNER,       MENU_TYPE_BOOL},
    [MENU_TUNER_SOFTMUTE]   = { MENU_SETUP_TUNER,       MENU_TYPE_BOOL},

    [MENU_SPECTURM_MODE]    = { MENU_SETUP_SPECTRUM,    MENU_TYPE_ENUM},
    [MENU_SPECTRUM_SPEED]   = { MENU_SETUP_SPECTRUM,    MENU_TYPE_NUMBER},

    [MENU_DISPLAY_BR_STBY]  = { MENU_SETUP_DISPLAY,     MENU_TYPE_NUMBER},

    [MENU_INPUT_ENC_RES]    = { MENU_SETUP_INPUT,       MENU_TYPE_NUMBER},
};

Menu *menuGet(void)
{
    return &menu;
}

void menuSetActive(MenuIdx index)
{
    if (menu.active == index)
        return;

    menu.active = (index != MENU_NULL) ? index : menu.parent;
    menu.parent = menuItems[index].parent;
    menu.dispOft = 0;

    uint8_t idx;

    for (idx = 0; idx < MENU_MAX_LEN; idx++) {
        menu.list[idx] = 0;
    }

    idx = 0;
    if (menu.parent != MENU_NULL && menu.parent != MENU_SETUP) {
        menu.list[idx++] = MENU_NULL;
        menu.active = MENU_NULL;
    }
    for (MenuIdx item = 0; item < MENU_END; item++) {
        if ((menuItems[item].parent == menu.parent) && item) {
            menu.list[idx++] = item;
            if (idx >= MENU_MAX_LEN)
                break;
        }
    }

    menu.listSize = idx;
}

MenuIdx menuGetParent(MenuIdx index)
{
    return menuItems[index].parent;
}

MenuIdx menuGetFirstChild(void)
{
    if (menu.active == MENU_NULL)
        return menu.parent;

    for (MenuIdx item = 0; item < MENU_END; item++) {
        if (menuItems[item].parent == menu.active) {
            return item;
        }
    }
    return menu.active;
}

void menuMove(int8_t diff)
{
    diff > 0 ? diff = 1 : diff < 0 ? diff = -1 : 0;

    int8_t newIdx = 0;

    for (int8_t idx = 0; idx < MENU_MAX_LEN; idx++) {
        if (menu.list[idx] == menu.active) {
            newIdx = idx;
            break;
        }
    }
    newIdx = (newIdx + menu.listSize + diff) % menu.listSize;

    menu.active = menu.list[newIdx];

    // Recalculate offset if needed
    if (menu.dispOft < newIdx - (menu.dispSize - 1)) {
        menu.dispOft = newIdx - (menu.dispSize - 1);
    } else if (menu.dispOft > newIdx) {
        menu.dispOft = newIdx;
    }
}

char *menuGetName(MenuIdx index)
{
    const char **txtLabels = labelsGet();

    return (char *)txtLabels[LABEL_MENU + (index - MENU_NULL)];
}

MenuType menuGetType(MenuIdx index)
{
    return menuItems[index].type;
}
