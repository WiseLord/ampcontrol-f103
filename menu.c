#include "menu.h"
#include "tr/labels.h"

#include "audio/audio.h"
#include "display/glcd.h"
#include "tuner/tuner.h"
#include "eemul.h"

static Menu menu;

typedef struct {
    MenuIdx parent;
    MenuType type;
} MenuItem;

static const MenuItem menuItems[MENU_END] = {
    //                          parent menu
    [MENU_NULL]             = {MENU_NULL,               MENU_TYPE_PARENT},

    [MENU_SETUP]            = {MENU_NULL,               MENU_TYPE_PARENT},

    [MENU_SETUP_AUDIO]      = {MENU_SETUP,              MENU_TYPE_PARENT},
    [MENU_SETUP_TUNER]      = {MENU_SETUP,              MENU_TYPE_PARENT},
    [MENU_SETUP_SPECTRUM]   = {MENU_SETUP,              MENU_TYPE_PARENT},
    [MENU_SETUP_DISPLAY]    = {MENU_SETUP,              MENU_TYPE_PARENT},
    [MENU_SETUP_INPUT]      = {MENU_SETUP,              MENU_TYPE_PARENT},

    [MENU_AUDIO_IC]         = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM, },

    [MENU_TUNER_IC]         = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM},
    [MENU_TUNER_BAND]       = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM},
    [MENU_TUNER_STEP]       = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM},
    [MENU_TUNER_MONO]       = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL},
    [MENU_TUNER_RDS]        = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL},
    [MENU_TUNER_BASS]       = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL},
    [MENU_TUNER_DE]         = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL},
    [MENU_TUNER_SOFTMUTE]   = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL},

    [MENU_SPECTURM_MODE]    = {MENU_SETUP_SPECTRUM,     MENU_TYPE_ENUM},
    [MENU_SPECTRUM_SPEED]   = {MENU_SETUP_SPECTRUM,     MENU_TYPE_ENUM},

    [MENU_DISPLAY_BR_STBY]  = {MENU_SETUP_DISPLAY,      MENU_TYPE_NUMBER},

    [MENU_INPUT_ENC_RES]    = {MENU_SETUP_INPUT,        MENU_TYPE_NUMBER},
};

static void menuMove(int8_t diff)
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

static int16_t menuGetValue(MenuIdx index)
{
    int16_t ret = 0;

    switch (index) {
    case MENU_AUDIO_IC:
        ret = audioProcGet()->ic;
        break;
    case MENU_TUNER_IC:
        ret = tunerGet()->ic;
        break;

    case MENU_TUNER_MONO:
        ret = (tunerGet()->flags & TUNER_FLAG_MONO) ? 1 : 0;
        break;
    case MENU_TUNER_RDS:
        ret = (tunerGet()->flags & TUNER_FLAG_RDS) ? 1 : 0;
        break;
    case MENU_TUNER_BASS:
        ret = (tunerGet()->flags & TUNER_FLAG_BASS) ? 1 : 0;
        break;
    default:
        ret = 0;
        break;
    }

    return ret;
}

static void menuStoreCurrentValue(void)
{
    switch (menu.active) {
    case MENU_AUDIO_IC:
        audioProcGet()->ic = menu.value;
        eeUpdate(EE_AUDIO_IC, audioProcGet()->ic);
        break;
    case MENU_TUNER_IC:
        tunerGet()->ic = menu.value;
        eeUpdate(EE_TUNER_IC, tunerGet()->ic);
        break;

    case MENU_TUNER_MONO:
        menu.value ? (tunerGet()->flags |= TUNER_FLAG_MONO) : (tunerGet()->flags &= ~TUNER_FLAG_MONO);
        eeUpdate(EE_TUNER_FLAGS, tunerGet()->flags);
        break;
    case MENU_TUNER_RDS:
        menu.value ? (tunerGet()->flags |= TUNER_FLAG_RDS) : (tunerGet()->flags &= ~TUNER_FLAG_RDS);
        eeUpdate(EE_TUNER_FLAGS, tunerGet()->flags);
        break;
    case MENU_TUNER_BASS:
        menu.value ? (tunerGet()->flags |= TUNER_FLAG_BASS) : (tunerGet()->flags &= ~TUNER_FLAG_BASS);
        eeUpdate(EE_TUNER_FLAGS, tunerGet()->flags);
        break;
    default:
        break;
    }
}

static void menuValueChange(int8_t diff)
{
    if (menuItems[menu.active].type == MENU_TYPE_BOOL) {
        if (diff)
            menu.value = !menu.value;
        return;
    }

    menu.value += diff;

    switch (menu.active) {
    case MENU_AUDIO_IC:
        if (menu.value >= AUDIO_IC_END)
            menu.value = AUDIO_IC_NO;
        if (menu.value < AUDIO_IC_NO)
            menu.value = AUDIO_IC_END - 1;
        break;
    case MENU_TUNER_IC:
        if (menu.value >= TUNER_IC_END)
            menu.value = TUNER_IC_NO;
        if (menu.value < TUNER_IC_NO)
            menu.value = TUNER_IC_END - 1;
        break;
    default:
        break;
    }
}

Menu *menuGet(void)
{
    return &menu;
}

void menuSetActive(MenuIdx index)
{
    if (menu.active == index) {
        menu.selected = !menu.selected;

        if (menu.selected)
            menu.value = menuGetValue(menu.active);
        else
            menuStoreCurrentValue();

        return;
    }

    menu.selected = 0;

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


void menuChange(int8_t diff)
{
    if (menu.selected) {
        menuValueChange(diff);
    } else {
        menuMove(diff);
    }
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

char *menuGetName(MenuIdx index)
{
    const char **txtLabels = labelsGet();

    return (char *)txtLabels[LABEL_MENU + (index - MENU_NULL)];
}

char *menuGetValueStr(MenuIdx index)
{
    char **labels = (char **)labelsGet();
    char *ret = ">";

    // Parent menu type
    if (menuItems[index].type == MENU_TYPE_PARENT) {
        return (index == MENU_NULL) ? "" : ret;
    }

    int16_t value = menuGetValue(index);

    if (index == menu.active && menu.selected)
        value = menu.value;

    // Bool menu type
    if (menuItems[index].type == MENU_TYPE_BOOL) {
        ret = labels[LABEL_BOOL_OFF + value];
        return ret;
    }

    // Enum menu types
    switch (index) {
    case MENU_AUDIO_IC:
        ret = labels[LABEL_AUDIO_IC + value];
        break;
    case MENU_TUNER_IC:
        ret = labels[LABEL_TUNER_IC + value];
        break;
    default:
        ret = "--";
        break;
    }

    return ret;
}
