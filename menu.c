#include "menu.h"
#include "tr/labels.h"

#include "audio/audio.h"
#include "display/glcd.h"
#include "tuner/tuner.h"
#include "eemul.h"

#define GENERATE_MENU_ITEM(CMD)    [MENU_RC_ ## CMD] = {MENU_SETUP_RC, MENU_TYPE_RC},

static Menu menu;

typedef struct {
    MenuIdx parent;
    MenuType type;
} MenuItem;

static const MenuItem menuItems[MENU_END] = {
//   menu index                parent menu              menu type
    [MENU_NULL]             = {MENU_NULL,               MENU_TYPE_PARENT},

    [MENU_SETUP]            = {MENU_NULL,               MENU_TYPE_PARENT},

    [MENU_SETUP_LANG]       = {MENU_SETUP,              MENU_TYPE_ENUM},
    [MENU_SETUP_AUDIO]      = {MENU_SETUP,              MENU_TYPE_PARENT},
    [MENU_SETUP_TUNER]      = {MENU_SETUP,              MENU_TYPE_PARENT},
    [MENU_SETUP_SPECTRUM]   = {MENU_SETUP,              MENU_TYPE_PARENT},
    [MENU_SETUP_DISPLAY]    = {MENU_SETUP,              MENU_TYPE_PARENT},
    [MENU_SETUP_INPUT]      = {MENU_SETUP,              MENU_TYPE_PARENT},
    [MENU_SETUP_RC]         = {MENU_SETUP,              MENU_TYPE_PARENT},

    [MENU_AUDIO_IC]         = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM},

    [MENU_TUNER_IC]         = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM},
    [MENU_TUNER_BAND]       = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM},
    [MENU_TUNER_STEP]       = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM},
    [MENU_TUNER_DEEMPH]     = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM},
    [MENU_TUNER_FMONO]      = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL},
    [MENU_TUNER_RDS]        = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL},
    [MENU_TUNER_BASS]       = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL},
    [MENU_TUNER_VOLUME]     = {MENU_SETUP_TUNER,        MENU_TYPE_NUMBER},

    [MENU_SPECTURM_MODE]    = {MENU_SETUP_SPECTRUM,     MENU_TYPE_ENUM},
    [MENU_SPECTRUM_SPEED]   = {MENU_SETUP_SPECTRUM,     MENU_TYPE_ENUM},

    [MENU_DISPLAY_BR_STBY]  = {MENU_SETUP_DISPLAY,      MENU_TYPE_NUMBER},

    [MENU_INPUT_ENC_RES]    = {MENU_SETUP_INPUT,        MENU_TYPE_NUMBER},

    FOREACH_CMD(GENERATE_MENU_ITEM)
};

static void menuMove(int8_t diff)
{
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
    AudioProc *aproc = audioGet();
    Tuner *tuner = tunerGet();
    TunerParam *tPar = tunerGetPar();

    switch (index) {
    case MENU_SETUP_LANG:
        ret = labelsGetLang();
        break;

    case MENU_AUDIO_IC:
        ret = aproc->ic;
        break;

    case MENU_TUNER_IC:
        ret = tuner->ic;
        break;
    case MENU_TUNER_BAND:
        ret = tPar->band;
        break;
    case MENU_TUNER_STEP:
        ret = tPar->step;
        break;
    case MENU_TUNER_DEEMPH:
        ret = tPar->deemph;
        break;

    case MENU_TUNER_FMONO:
        ret = (tPar->flags & TUNER_FLAG_MONO) ? 1 : 0;
        break;
    case MENU_TUNER_RDS:
        ret = (tPar->flags & TUNER_FLAG_RDS) ? 1 : 0;
        break;
    case MENU_TUNER_BASS:
        ret = (tPar->flags & TUNER_FLAG_BASS) ? 1 : 0;
        break;

    case MENU_TUNER_VOLUME:
        ret = tPar->volume;
        break;
    default:
        ret = 0;
        break;
    }

    if (index >= MENU_RC_STBY_SWITCH && index < MENU_RC_STBY_SWITCH + RC_CMD_END) {
        ret = rcGetCode(index - MENU_RC_STBY_SWITCH);
    }

    return ret;
}

static void menuStoreCurrentValue(void)
{
    AudioProc *aproc = audioGet();
    Tuner *tuner = tunerGet();
    TunerParam *tPar = tunerGetPar();

    switch (menu.active) {
    case MENU_SETUP_LANG:
        labelsSetLang(menu.value);
        eeUpdate(EE_LANGUAGE, labelsGetLang());
        break;

    case MENU_AUDIO_IC:
        aproc->ic = menu.value;
        eeUpdate(EE_AUDIO_IC, aproc->ic);
        break;

    case MENU_TUNER_IC:
        tuner->ic = menu.value;
        eeUpdate(EE_TUNER_IC, tuner->ic);
        break;
    case MENU_TUNER_BAND:
        tPar->band = menu.value;
        eeUpdate(EE_TUNER_BAND, tPar->band);
        break;
    case MENU_TUNER_STEP:
        tPar->step = menu.value;
        eeUpdate(EE_TUNER_STEP, tPar->step);
        break;
    case MENU_TUNER_DEEMPH:
        tPar->deemph = menu.value;
        eeUpdate(EE_TUNER_DEEMPH, tPar->deemph);
        break;

    case MENU_TUNER_FMONO:
        menu.value ? (tPar->flags |= TUNER_FLAG_MONO) : (tPar->flags &= ~TUNER_FLAG_MONO);
        eeUpdate(EE_TUNER_FLAGS, tPar->flags);
        break;
    case MENU_TUNER_RDS:
        menu.value ? (tPar->flags |= TUNER_FLAG_RDS) : (tPar->flags &= ~TUNER_FLAG_RDS);
        eeUpdate(EE_TUNER_FLAGS, tPar->flags);
        break;
    case MENU_TUNER_BASS:
        menu.value ? (tPar->flags |= TUNER_FLAG_BASS) : (tPar->flags &= ~TUNER_FLAG_BASS);
        eeUpdate(EE_TUNER_FLAGS, tPar->flags);
        break;

    case MENU_TUNER_VOLUME:
        tPar->volume = menu.value;
        eeUpdate(EE_TUNER_VOLUME, tPar->volume);
        break;
    default:
        break;
    }


    if (menu.active >= MENU_RC_STBY_SWITCH && menu.active < MENU_RC_STBY_SWITCH + RC_CMD_END) {
        rcSaveCode(menu.active - MENU_RC_STBY_SWITCH, (uint16_t)menu.value);
    }

}

static void menuValueChange(int8_t diff)
{
    if (menuItems[menu.active].type == MENU_TYPE_BOOL) {
        if (diff)
            menu.value = !menu.value;
        return;
    }

    if (menuItems[menu.active].type == MENU_TYPE_RC) {
        RcData rcData = rcRead(false);

        menu.value = ((rcData.addr & 0xFF) << 8) | rcData.cmd;
        return;
    }

    menu.value += diff;

    switch (menu.active) {
    case MENU_SETUP_LANG:
        if (menu.value >= LANG_END)
            menu.value = LANG_END - 1;
        if (menu.value < LANG_DEFAULT)
            menu.value = LANG_DEFAULT;
        break;
    case MENU_AUDIO_IC:
        if (menu.value >= AUDIO_IC_END)
            menu.value = AUDIO_IC_END - 1;
        if (menu.value < AUDIO_IC_NO)
            menu.value = AUDIO_IC_NO;
        break;
    case MENU_TUNER_IC:
        if (menu.value >= TUNER_IC_END)
            menu.value = TUNER_IC_END - 1;
        if (menu.value < TUNER_IC_NO)
            menu.value = TUNER_IC_NO;
        break;
    case MENU_TUNER_BAND:
        if (menu.value >= TUNER_BAND_END)
            menu.value = TUNER_BAND_END - 1;
        if (menu.value < TUNER_BAND_FM_US_EUROPE)
            menu.value = TUNER_BAND_FM_US_EUROPE;
        break;
    case MENU_TUNER_STEP:
        if (menu.value >= TUNER_STEP_END)
            menu.value = TUNER_STEP_END - 1;
        if (menu.value < TUNER_STEP_50K)
            menu.value = TUNER_STEP_50K;
        break;
    case MENU_TUNER_DEEMPH:
        if (menu.value >= TUNER_DEEMPH_END)
            menu.value = TUNER_DEEMPH_END - 1;
        if (menu.value < TUNER_DEEMPH_50u)
            menu.value = TUNER_DEEMPH_50u;
        break;
    case MENU_TUNER_VOLUME:
        if (menu.value > TUNER_VOLUME_MAX)
            menu.value = TUNER_VOLUME_MAX;
        if (menu.value < TUNER_VOLUME_MIN)
            menu.value = TUNER_VOLUME_MIN;
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
    if (diff > 0)
        diff = 1;
    else if (diff < 0)
        diff = -1;

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

    if (menuItems[index].type == MENU_TYPE_NUMBER) {
        ret = glcdPrepareNum(value, 5, ' ', 10);
        return ret;
    }

    if (menuItems[index].type == MENU_TYPE_RC) {
        ret = glcdPrepareNum((uint16_t)value, 4, '0', 16);
        return ret;
    }

    // Enum menu types
    switch (index) {
    case MENU_SETUP_LANG:
        ret = labels[LABEL_LANG + value];
        break;
    case MENU_AUDIO_IC:
        ret = labels[LABEL_AUDIO_IC + value];
        break;
    case MENU_TUNER_IC:
        ret = labels[LABEL_TUNER_IC + value];
        break;
    case MENU_TUNER_BAND:
        ret = labels[LABEL_TUNER_BAND + value];
        break;
    case MENU_TUNER_STEP:
        ret = labels[LABEL_TUNER_STEP + value];
        break;
    case MENU_TUNER_DEEMPH:
        ret = labels[LABEL_TUNER_DEEMPH + value];
        break;
    default:
        ret = "--";
        break;
    }

    return ret;
}
