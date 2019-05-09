#include "menu.h"
#include "tr/labels.h"

#include "audio/audio.h"
#include "display/glcd.h"
#include "eemap.h"
#include "gui/layout.h"
#include "input.h"
#include "pins.h"
#include "screen.h"
#include "settings.h"
#include "spectrum.h"
#include "tuner/tuner.h"

#define GENERATE_MENU_ITEM(CMD)    [MENU_RC_ ## CMD] = {MENU_SETUP_RC, MENU_TYPE_RC, EE_RC_ ## CMD},

static Menu menu;

typedef struct {
    MenuIdx parent;
    MenuType type;
    EE_Param cell;
} MenuItem;

static const MenuItem menuItems[MENU_END] = {
//   menu index                parent menu              menu type           ee cell
    [MENU_NULL]             = {MENU_NULL,               MENU_TYPE_PARENT,   EE_NULL},

    [MENU_SETUP]            = {MENU_NULL,               MENU_TYPE_PARENT,   EE_NULL},

    [MENU_SETUP_SYSTEM]     = {MENU_SETUP,              MENU_TYPE_PARENT,   EE_NULL},
    [MENU_SETUP_AUDIO]      = {MENU_SETUP,              MENU_TYPE_PARENT,   EE_NULL},
    [MENU_SETUP_TUNER]      = {MENU_SETUP,              MENU_TYPE_PARENT,   EE_NULL},
    [MENU_SETUP_SPECTRUM]   = {MENU_SETUP,              MENU_TYPE_PARENT,   EE_NULL},
    [MENU_SETUP_DISPLAY]    = {MENU_SETUP,              MENU_TYPE_PARENT,   EE_NULL},
    [MENU_SETUP_ALARM]      = {MENU_SETUP,              MENU_TYPE_PARENT,   EE_NULL},
    [MENU_SETUP_RC]         = {MENU_SETUP,              MENU_TYPE_PARENT,   EE_NULL},

    [MENU_SYSTEM_LANG]      = {MENU_SETUP_SYSTEM,       MENU_TYPE_ENUM,     EE_SYSTEM_LANG},
    [MENU_SYSTEM_MUTESTBY]  = {MENU_SETUP_SYSTEM,       MENU_TYPE_ENUM,     EE_SYSTEM_MUTESTBY},
    [MENU_SYSTEM_ENC_RES]   = {MENU_SETUP_SYSTEM,       MENU_TYPE_NUMBER,   EE_SYSTEM_ENC_RES},
    [MENU_SYSTEM_SIL_TIM]   = {MENU_SETUP_SYSTEM,       MENU_TYPE_NUMBER,   EE_SYSTEM_SIL_TIM},
    [MENU_SYSTEM_RTC_CORR]  = {MENU_SETUP_SYSTEM,       MENU_TYPE_NUMBER,   EE_SYSTEM_RTC_CORR},

    [MENU_AUDIO_IC]         = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     EE_AUDIO_IC},
    [MENU_AUDIO_IN_0]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     EE_AUDIO_IN0},
    [MENU_AUDIO_IN_1]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     EE_AUDIO_IN1},
    [MENU_AUDIO_IN_2]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     EE_AUDIO_IN2},
    [MENU_AUDIO_IN_3]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     EE_AUDIO_IN3},
    [MENU_AUDIO_IN_4]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     EE_AUDIO_IN4},
    [MENU_AUDIO_IN_5]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     EE_AUDIO_IN5},
    [MENU_AUDIO_IN_6]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     EE_AUDIO_IN6},
    [MENU_AUDIO_IN_7]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     EE_AUDIO_IN7},

    [MENU_TUNER_IC]         = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     EE_TUNER_IC},
    [MENU_TUNER_BAND]       = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     EE_TUNER_BAND},
    [MENU_TUNER_STEP]       = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     EE_TUNER_STEP},
    [MENU_TUNER_DEEMPH]     = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     EE_TUNER_DEEMPH},
    [MENU_TUNER_MODE]       = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     EE_TUNER_MODE},
    [MENU_TUNER_FMONO]      = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL,     EE_TUNER_FMONO},
    [MENU_TUNER_RDS]        = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL,     EE_TUNER_RDS},
    [MENU_TUNER_BASS]       = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL,     EE_TUNER_BASS},
    [MENU_TUNER_VOLUME]     = {MENU_SETUP_TUNER,        MENU_TYPE_NUMBER,   EE_TUNER_VOLUME},

    [MENU_ALARM_HOUR]       = {MENU_SETUP_ALARM,        MENU_TYPE_NUMBER,   EE_ALARM_HOUR},
    [MENU_ALARM_MINUTE]     = {MENU_SETUP_ALARM,        MENU_TYPE_NUMBER,   EE_ALARM_MINUTE},
    [MENU_ALARM_DAYS]       = {MENU_SETUP_ALARM,        MENU_TYPE_ENUM,     EE_ALARM_DAYS},

    [MENU_SPECTURM_MODE]    = {MENU_SETUP_SPECTRUM,     MENU_TYPE_ENUM,     EE_SPECTRUM_MODE},

    [MENU_DISPLAY_BR_STBY]  = {MENU_SETUP_DISPLAY,      MENU_TYPE_NUMBER,   EE_DISPLAY_BR_STBY},
    [MENU_DISPLAY_BR_WORK]  = {MENU_SETUP_DISPLAY,      MENU_TYPE_NUMBER,   EE_DISPLAY_BR_WORK},
    [MENU_DISPLAY_ROTATE]   = {MENU_SETUP_DISPLAY,      MENU_TYPE_BOOL,     EE_DISPLAY_ROTATE},

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
    newIdx = (int8_t)((newIdx + menu.listSize + diff) % menu.listSize);

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
    AudioProc *aProc = audioGet();
    Tuner *tuner = tunerGet();
    TunerParam *tPar = &tuner->par;
    Spectrum *sp = spGet();

    switch (index) {
    case MENU_SYSTEM_LANG:
        ret = (int16_t)(labelsGetLang());
        break;

    case MENU_AUDIO_IC:
        ret = (int16_t)aProc->par.ic;
        break;
    case MENU_AUDIO_IN_0:
    case MENU_AUDIO_IN_1:
    case MENU_AUDIO_IN_2:
    case MENU_AUDIO_IN_3:
    case MENU_AUDIO_IN_4:
    case MENU_AUDIO_IN_5:
    case MENU_AUDIO_IN_6:
    case MENU_AUDIO_IN_7:
        ret = aProc->par.inType[index - MENU_AUDIO_IN_0];
        break;

    case MENU_TUNER_IC:
        ret = (int16_t)(tPar->ic);
        break;
    case MENU_TUNER_BAND:
        ret = (int16_t)(tPar->band);
        break;
    case MENU_TUNER_STEP:
        ret = (int16_t)(tPar->step);
        break;
    case MENU_TUNER_DEEMPH:
        ret = (int16_t)(tPar->deemph);
        break;
    case MENU_TUNER_MODE:
        ret = (int16_t)(tPar->mode);
        break;

    case MENU_TUNER_FMONO:
        ret = tPar->forcedMono;
        break;
    case MENU_TUNER_RDS:
        ret = tPar->rds;
        break;
    case MENU_TUNER_BASS:
        ret = tPar->bassBoost;
        break;

    case MENU_TUNER_VOLUME:
        ret = tPar->volume;
        break;

    case MENU_SPECTURM_MODE:
        ret = (int16_t)(sp->mode);
        break;

    case MENU_DISPLAY_BR_STBY:
    case MENU_DISPLAY_BR_WORK:
        ret = screenGetBrightness(BR_STBY + index - MENU_DISPLAY_BR_STBY);
        break;

    case MENU_DISPLAY_ROTATE:
        ret = glcdGetRotate();
        break;

    case MENU_SYSTEM_ENC_RES:
        ret = inputGetEncRes();
        break;

    default:
        ret = settingsGet(menuItems[index].cell);
        break;
    }

    if (index >= MENU_RC_STBY_SWITCH && index < MENU_RC_STBY_SWITCH + RC_CMD_END) {
        ret = (int16_t)rcGetCode(index - MENU_RC_STBY_SWITCH);
    }

    return ret;
}

static void menuStoreCurrentValue(void)
{
    AudioProc *aProc = audioGet();
    Tuner *tuner = tunerGet();
    TunerParam *tPar = &tuner->par;
    Spectrum *sp = spGet();

    switch (menu.active) {
    case MENU_SYSTEM_LANG:
        labelsSetLang((Lang)(menu.value));
        break;
    case MENU_SYSTEM_MUTESTBY:
        pinsInitMuteStby((MuteStby)menu.value);
        break;
    case MENU_SYSTEM_ENC_RES:
        inputSetEncRes((int8_t)menu.value);
        break;
    case MENU_SYSTEM_RTC_CORR:
        rtcSetCorrection(menu.value);
        break;

    case MENU_AUDIO_IC:
        aProc->par.ic = (AudioIC)(menu.value);
        break;
    case MENU_AUDIO_IN_0:
    case MENU_AUDIO_IN_1:
    case MENU_AUDIO_IN_2:
    case MENU_AUDIO_IN_3:
    case MENU_AUDIO_IN_4:
    case MENU_AUDIO_IN_5:
    case MENU_AUDIO_IN_6:
    case MENU_AUDIO_IN_7:
        aProc->par.inType[menu.active - MENU_AUDIO_IN_0] = (InputType)menu.value;
        break;

    case MENU_TUNER_IC:
        tPar->ic = (TunerIC)menu.value;
        break;
    case MENU_TUNER_BAND:
        tPar->band = (TunerBand)menu.value;
        break;
    case MENU_TUNER_STEP:
        tPar->step = (TunerStep)menu.value;
        break;
    case MENU_TUNER_DEEMPH:
        tPar->deemph = (TunerDeemph)menu.value;
        break;
    case MENU_TUNER_MODE:
        tPar->mode = (TunerMode)menu.value;
        break;

    case MENU_TUNER_FMONO:
        tPar->forcedMono = (bool)menu.value;
        break;
    case MENU_TUNER_RDS:
        tPar->rds = (bool)menu.value;
        break;
    case MENU_TUNER_BASS:
        tPar->bassBoost = (bool)menu.value;
        break;

    case MENU_TUNER_VOLUME:
        tPar->volume = (int8_t)(menu.value);
        break;

    case MENU_SPECTURM_MODE:
        sp->mode = (SpMode)(menu.value);
        break;

    case MENU_DISPLAY_BR_STBY:
    case MENU_DISPLAY_BR_WORK:
        screenSetBrightness(BR_STBY + menu.active - MENU_DISPLAY_BR_STBY, (int8_t)menu.value);
        break;

    case MENU_DISPLAY_ROTATE:
        glcdRotate((bool)menu.value);
        canvasClear();
        break;

    default:
        break;
    }

    if (menu.active >= MENU_RC_STBY_SWITCH && menu.active < MENU_RC_STBY_SWITCH + RC_CMD_END) {
        rcSaveCode((uint16_t)(menu.active - MENU_RC_STBY_SWITCH), (uint16_t)menu.value);
    }

    if (menu.active < MENU_END) {
        EE_Param cell = menuItems[menu.active].cell;
        settingsSet(cell, menu.value);
        if (EE_NULL != cell) {
            eeUpdate(menuItems[menu.active].cell, menu.value);
        }
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

        menu.value = (int16_t)(((rcData.addr & 0xFF) << 8) | rcData.cmd);
        return;
    }

    menu.value += diff;

    switch (menu.active) {
    case MENU_SYSTEM_LANG:
        if (menu.value > LANG_END - 1)
            menu.value = LANG_END - 1;
        if (menu.value < LANG_DEFAULT)
            menu.value = LANG_DEFAULT;
        break;
    case MENU_SYSTEM_MUTESTBY:
        if (menu.value > MUTESTBY_END - 1)
            menu.value = MUTESTBY_END - 1;
        if (menu.value < MUTESTBY_SWD)
            menu.value = MUTESTBY_SWD;
        break;
    case MENU_SYSTEM_ENC_RES:
        if (menu.value > ENC_RES_MAX)
            menu.value = ENC_RES_MAX;
        if (menu.value < ENC_RES_MIN)
            menu.value = ENC_RES_MIN;
        break;
    case MENU_SYSTEM_SIL_TIM:
        if (menu.value > 60)
            menu.value = 60;
        if (menu.value < 0)
            menu.value = 0;
        break;
    case MENU_SYSTEM_RTC_CORR:
        if (menu.value > 20)
            menu.value = 20;
        if (menu.value < -20)
            menu.value = -20;
        break;

    case MENU_AUDIO_IC:
        if (menu.value > AUDIO_IC_END - 1)
            menu.value = AUDIO_IC_END - 1;
        if (menu.value < AUDIO_IC_NO)
            menu.value = AUDIO_IC_NO;
        break;
    case MENU_AUDIO_IN_0:
    case MENU_AUDIO_IN_1:
    case MENU_AUDIO_IN_2:
    case MENU_AUDIO_IN_3:
    case MENU_AUDIO_IN_4:
    case MENU_AUDIO_IN_5:
    case MENU_AUDIO_IN_6:
    case MENU_AUDIO_IN_7:
        if (menu.value > IN_END - 1)
            menu.value = IN_END - 1;
        if (menu.value < IN_TUNER)
            menu.value = IN_TUNER;
        break;

    case MENU_TUNER_IC:
        if (menu.value > TUNER_IC_END - 1)
            menu.value = TUNER_IC_END - 1;
        if (menu.value < TUNER_IC_NO)
            menu.value = TUNER_IC_NO;
        break;
    case MENU_TUNER_BAND:
        if (menu.value > TUNER_BAND_END - 1)
            menu.value = TUNER_BAND_END - 1;
        if (menu.value < TUNER_BAND_FM_US_EUROPE)
            menu.value = TUNER_BAND_FM_US_EUROPE;
        break;
    case MENU_TUNER_STEP:
        if (menu.value > TUNER_STEP_END - 1)
            menu.value = TUNER_STEP_END - 1;
        if (menu.value < TUNER_STEP_50K)
            menu.value = TUNER_STEP_50K;
        break;
    case MENU_TUNER_DEEMPH:
        if (menu.value > TUNER_DEEMPH_END - 1)
            menu.value = TUNER_DEEMPH_END - 1;
        if (menu.value < TUNER_DEEMPH_50u)
            menu.value = TUNER_DEEMPH_50u;
        break;
    case MENU_TUNER_MODE:
        if (menu.value > TUNER_MODE_END - 1)
            menu.value = TUNER_MODE_END - 1;
        if (menu.value < TUNER_MODE_GRID)
            menu.value = TUNER_MODE_GRID;
        break;
    case MENU_TUNER_VOLUME:
        if (menu.value > TUNER_VOLUME_MAX)
            menu.value = TUNER_VOLUME_MAX;
        if (menu.value < TUNER_VOLUME_MIN)
            menu.value = TUNER_VOLUME_MIN;
        break;

    case MENU_SPECTURM_MODE:
        if (menu.value > SP_MODE_END - 1)
            menu.value = SP_MODE_END - 1;
        if (menu.value < SP_MODE_STEREO)
            menu.value = SP_MODE_STEREO;
        break;

    case MENU_DISPLAY_BR_STBY:
    case MENU_DISPLAY_BR_WORK:
        if (menu.value > LCD_BR_MAX)
            menu.value = LCD_BR_MAX;
        if (menu.value < LCD_BR_MIN)
            menu.value = LCD_BR_MIN;
        break;

    case MENU_ALARM_HOUR:
        if (menu.value > 23)
            menu.value = 0;
        if (menu.value < 0)
            menu.value = 23;
        break;
    case MENU_ALARM_MINUTE:
        if (menu.value > 59)
            menu.value = 0;
        if (menu.value < 0)
            menu.value = 59;
        break;
    case MENU_ALARM_DAYS:
        if (menu.value > ALARM_DAY_END - 1)
            menu.value = ALARM_DAY_END - 1;
        if (menu.value < ALARM_DAY_OFF)
            menu.value = ALARM_DAY_OFF;
        break;
    default:
        break;
    }

    if (menu.active == MENU_DISPLAY_BR_STBY || menu.active == MENU_DISPLAY_BR_WORK) {
        dispdrvSetBrightness((int8_t)menu.value);
    }
}

static void menuSelect(MenuIdx index)
{
    menu.selected = 0;

    menu.active = (index != MENU_NULL) ? index : menu.parent;
    menu.parent = menuItems[index].parent;
    menu.dispOft = 0;

    uint8_t idx;

    for (idx = 0; idx < MENU_MAX_LEN; idx++) {
        menu.list[idx] = 0;
    }

    idx = 0;
    if (!menuIsTop()) {
        menu.list[idx++] = MENU_NULL;
        menu.active = MENU_NULL;
    }
    for (MenuIdx item = 0; item < MENU_END; item++) {
        if ((menuItems[item].parent == menu.parent) && item) {
            menu.list[idx++] = (uint8_t)item;
            if (idx >= MENU_MAX_LEN)
                break;
        }
    }

    menu.listSize = idx;
}

Menu *menuGet(void)
{
    return &menu;
}

void menuSetActive(MenuIdx index)
{
    if (menu.active == index) {
        menu.selected = !menu.selected;

        if (menu.selected) {
            menu.value = menuGetValue(menu.active);
            if (index == MENU_DISPLAY_BR_STBY || index == MENU_DISPLAY_BR_WORK) {
                dispdrvSetBrightness(screenGetBrightness(BR_STBY + index - MENU_DISPLAY_BR_STBY));
            }
        } else {
            menuStoreCurrentValue();
            if (index == MENU_DISPLAY_BR_STBY || index == MENU_DISPLAY_BR_WORK) {
                dispdrvSetBrightness(screenGetBrightness(BR_WORK));
            }
        }

        return;
    }

    menuSelect(index);
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

bool menuIsTop(void)
{
    // TODO: Top menu on first selection instead of MENU_SETUP
    return (menu.parent == MENU_NULL || menu.parent == MENU_SETUP);
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

const char *menuGetName(MenuIdx index)
{
    return labelsGet((Label)(LABEL_MENU + (index - MENU_NULL)));
}

const char *menuGetValueStr(MenuIdx index)
{
    const char *ret = ">";
    static const char *noVal = "--";

    // Parent menu type
    if (menuItems[index].type == MENU_TYPE_PARENT) {
        return (index == MENU_NULL) ? "" : ret;
    }

    int16_t value = menuGetValue(index);

    if (index == menu.active && menu.selected)
        value = menu.value;

    // Bool menu type
    if (menuItems[index].type == MENU_TYPE_BOOL) {
        ret = labelsGet((Label)(LABEL_BOOL_OFF + value));
        return ret;
    }

    if (menuItems[index].type == MENU_TYPE_NUMBER) {
        ret = glcdPrepareNum(value, 5, ' ', 10);
        return ret;
    }

    if (menuItems[index].type == MENU_TYPE_RC) {
        if ((uint16_t)value == EE_NOT_FOUND) {
            ret = noVal;
        } else {
            ret = glcdPrepareNum((uint16_t)value, 4, '0', 16);
        }
        return ret;
    }

    // Enum menu types
    switch (index) {
    case MENU_SYSTEM_LANG:
        ret = labelsGet((Label)(LABEL_LANG + value));
        break;
    case MENU_SYSTEM_MUTESTBY:
        ret = labelsGet((Label)(LABEL_MUTESTBY + value));
        break;

    case MENU_AUDIO_IC:
        ret = labelsGet((Label)(LABEL_AUDIO_IC + value));
        break;
    case MENU_AUDIO_IN_0:
    case MENU_AUDIO_IN_1:
    case MENU_AUDIO_IN_2:
    case MENU_AUDIO_IN_3:
    case MENU_AUDIO_IN_4:
    case MENU_AUDIO_IN_5:
    case MENU_AUDIO_IN_6:
    case MENU_AUDIO_IN_7:
        ret = labelsGet((Label)(LABEL_IN_TUNER + value));
        break;
    case MENU_TUNER_IC:
        ret = labelsGet((Label)(LABEL_TUNER_IC + value));
        break;
    case MENU_TUNER_BAND:
        ret = labelsGet((Label)(LABEL_TUNER_BAND + value));
        break;
    case MENU_TUNER_STEP:
        ret = labelsGet((Label)(LABEL_TUNER_STEP + value));
        break;
    case MENU_TUNER_DEEMPH:
        ret = labelsGet((Label)(LABEL_TUNER_DEEMPH + value));
        break;
    case MENU_TUNER_MODE:
        ret = labelsGet((Label)(LABEL_TUNER_MODE + value));
        break;
    case MENU_SPECTURM_MODE:
        ret = labelsGet((Label)(LABEL_SPECTRUM_MODE + value));
        break;
    case MENU_ALARM_DAYS:
        ret = labelsGet((Label)(LABEL_ALARM_DAY + value));
        break;
    default:
        ret = noVal;
        break;
    }

    return ret;
}
