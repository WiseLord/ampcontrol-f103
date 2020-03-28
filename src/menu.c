#include "menu.h"

#include <string.h>

#include "amp.h"
#include "audio/audio.h"
#include "gui/canvas.h"
#include "i2cexp.h"
#include "input.h"
#include "rc.h"
#include "screen.h"
#include "settings.h"
#include "spectrum.h"
#include "tr/labels.h"
#include "tuner/tuner.h"
#include "utils.h"

#define GENERATE_MENU_ITEM(CMD)    [MENU_RC_ ## CMD] = {MENU_SETUP_RC, MENU_TYPE_RC, PARAM_RC_ ## CMD},

static Menu menu;

typedef struct {
    MenuIdx parent;
    MenuType type;
    Param param;
} MenuItem;

static const MenuItem menuItems[MENU_END] = {
//   menu index                parent menu              menu type           parameter
    [MENU_NULL]             = {MENU_NULL,               MENU_TYPE_PARENT,   PARAM_NULL},

    [MENU_SETUP]            = {MENU_NULL,               MENU_TYPE_PARENT,   PARAM_NULL},

    [MENU_SETUP_SYSTEM]     = {MENU_SETUP,              MENU_TYPE_PARENT,   PARAM_NULL},
    [MENU_SETUP_AUDIO]      = {MENU_SETUP,              MENU_TYPE_PARENT,   PARAM_NULL},
    [MENU_SETUP_TUNER]      = {MENU_SETUP,              MENU_TYPE_PARENT,   PARAM_NULL},
    [MENU_SETUP_SPECTRUM]   = {MENU_SETUP,              MENU_TYPE_PARENT,   PARAM_NULL},
    [MENU_SETUP_DISPLAY]    = {MENU_SETUP,              MENU_TYPE_PARENT,   PARAM_NULL},
    [MENU_SETUP_ALARM]      = {MENU_SETUP,              MENU_TYPE_PARENT,   PARAM_NULL},
    [MENU_SETUP_RC]         = {MENU_SETUP,              MENU_TYPE_PARENT,   PARAM_NULL},

    [MENU_SYSTEM_LANG]      = {MENU_SETUP_SYSTEM,       MENU_TYPE_ENUM,     PARAM_SYSTEM_LANG},
    [MENU_SYSTEM_MUTESTBY]  = {MENU_SETUP_SYSTEM,       MENU_TYPE_ENUM,     PARAM_SYSTEM_MUTESTBY},
    [MENU_SYSTEM_I2C_EXT]   = {MENU_SETUP_SYSTEM,       MENU_TYPE_PARENT,   PARAM_NULL},
    [MENU_SYSTEM_ENC_RES]   = {MENU_SETUP_SYSTEM,       MENU_TYPE_NUMBER,   PARAM_SYSTEM_ENC_RES},
    [MENU_SYSTEM_SIL_TIM]   = {MENU_SETUP_SYSTEM,       MENU_TYPE_NUMBER,   PARAM_SYSTEM_SIL_TIM},
    [MENU_SYSTEM_RTC_CORR]  = {MENU_SETUP_SYSTEM,       MENU_TYPE_NUMBER,   PARAM_SYSTEM_RTC_CORR},

    [MENU_I2C_EXT_IN_STAT]  = {MENU_SYSTEM_I2C_EXT,     MENU_TYPE_ENUM,     PARAM_I2C_EXT_IN_STAT},
    [MENU_I2C_EXT_BT]       = {MENU_SYSTEM_I2C_EXT,     MENU_TYPE_ENUM,     PARAM_I2C_EXT_BT},

    [MENU_AUDIO_IC]         = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     PARAM_AUDIO_IC},
    [MENU_AUDIO_SHOWDB]     = {MENU_SETUP_AUDIO,        MENU_TYPE_BOOL,     PARAM_AUDIO_SHOWDB},
    [MENU_AUDIO_IN_0]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     PARAM_AUDIO_IN0},
    [MENU_AUDIO_IN_1]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     PARAM_AUDIO_IN1},
    [MENU_AUDIO_IN_2]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     PARAM_AUDIO_IN2},
    [MENU_AUDIO_IN_3]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     PARAM_AUDIO_IN3},
    [MENU_AUDIO_IN_4]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     PARAM_AUDIO_IN4},
    [MENU_AUDIO_IN_5]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     PARAM_AUDIO_IN5},
    [MENU_AUDIO_IN_6]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     PARAM_AUDIO_IN6},
    [MENU_AUDIO_IN_7]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     PARAM_AUDIO_IN7},

    [MENU_TUNER_IC]         = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     PARAM_TUNER_IC},
    [MENU_TUNER_BAND]       = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     PARAM_TUNER_BAND},
    [MENU_TUNER_STEP]       = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     PARAM_TUNER_STEP},
    [MENU_TUNER_DEEMPH]     = {MENU_SETUP_TUNER,        MENU_TYPE_ENUM,     PARAM_TUNER_DEEMPH},
    [MENU_TUNER_STA_MODE]   = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL,     PARAM_TUNER_STA_MODE},
    [MENU_TUNER_FMONO]      = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL,     PARAM_TUNER_FMONO},
    [MENU_TUNER_RDS]        = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL,     PARAM_TUNER_RDS},
    [MENU_TUNER_BASS]       = {MENU_SETUP_TUNER,        MENU_TYPE_BOOL,     PARAM_TUNER_BASS},
    [MENU_TUNER_VOLUME]     = {MENU_SETUP_TUNER,        MENU_TYPE_NUMBER,   PARAM_TUNER_VOLUME},

    [MENU_ALARM_HOUR]       = {MENU_SETUP_ALARM,        MENU_TYPE_NUMBER,   PARAM_ALARM_HOUR},
    [MENU_ALARM_MINUTE]     = {MENU_SETUP_ALARM,        MENU_TYPE_NUMBER,   PARAM_ALARM_MINUTE},
    [MENU_ALARM_DAYS]       = {MENU_SETUP_ALARM,        MENU_TYPE_ENUM,     PARAM_ALARM_DAYS},

    [MENU_SPECTRUM_MODE]    = {MENU_SETUP_SPECTRUM,     MENU_TYPE_ENUM,     PARAM_SPECTRUM_MODE},
    [MENU_SPECTRUM_PEAKS]   = {MENU_SETUP_SPECTRUM,     MENU_TYPE_BOOL,     PARAM_SPECTRUM_PEAKS},
    [MENU_SPECTRUM_GRAD]    = {MENU_SETUP_SPECTRUM,     MENU_TYPE_BOOL,     PARAM_SPECTRUM_GRAD},

    [MENU_DISPLAY_BR_STBY]  = {MENU_SETUP_DISPLAY,      MENU_TYPE_NUMBER,   PARAM_DISPLAY_BR_STBY},
    [MENU_DISPLAY_BR_WORK]  = {MENU_SETUP_DISPLAY,      MENU_TYPE_NUMBER,   PARAM_DISPLAY_BR_WORK},
    [MENU_DISPLAY_ROTATE]   = {MENU_SETUP_DISPLAY,      MENU_TYPE_BOOL,     PARAM_DISPLAY_ROTATE},
    [MENU_DISPLAY_DEF]      = {MENU_SETUP_DISPLAY,      MENU_TYPE_ENUM,     PARAM_DISPLAY_DEF},
    [MENU_DISPLAY_PALETTE]  = {MENU_SETUP_DISPLAY,      MENU_TYPE_ENUM,     PARAM_DISPLAY_PALETTE},

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

    ret = settingsGet(menuItems[index].param);

    return ret;
}

static void menuStoreCurrentValue(void)
{
    if (menu.active < MENU_END) {
        Param param = menuItems[menu.active].param;
        settingsSet(param, menu.value);
        settingsStore(param, menu.value);
    }

    switch (menu.active) {
    case MENU_SYSTEM_MUTESTBY:
        ampPinMute(true);
        ampPinStby(true);
        break;
    case MENU_SYSTEM_RTC_CORR:
        rtcSetCorrection(menu.value);
        break;
    case MENU_DISPLAY_BR_STBY:
    case MENU_DISPLAY_BR_WORK:
        screenSetBrightness((int8_t)(menu.value));
        break;
    case MENU_DISPLAY_ROTATE:
        glcdRotate((bool)menu.value);
        canvasClear();
        break;
    case MENU_DISPLAY_PALETTE:
        canvasGet()->pal = paletteGet((PalIdx)menu.value);
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
        if (menu.value < MUTESTBY_POS)
            menu.value = MUTESTBY_POS;
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
        if (menu.value > 64)
            menu.value = 64;
        if (menu.value < -63)
            menu.value = -63;
        break;

    case MENU_I2C_EXT_IN_STAT:
    case MENU_I2C_EXT_BT:
        if (menu.value >= I2C_ADDR_END - 1)
            menu.value = I2C_ADDR_END - 1;
        if (menu.value < I2C_ADDR_DISABLED)
            menu.value = I2C_ADDR_DISABLED;
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
    case MENU_TUNER_VOLUME:
        if (menu.value > TUNER_VOLUME_MAX)
            menu.value = TUNER_VOLUME_MAX;
        if (menu.value < TUNER_VOLUME_MIN)
            menu.value = TUNER_VOLUME_MIN;
        break;

    case MENU_SPECTRUM_MODE:
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
    case MENU_DISPLAY_DEF:
        if (menu.value > SCREEN_STANDBY - 1)
            menu.value = SCREEN_STANDBY - 1;
        if (menu.value < SCREEN_SPECTRUM)
            menu.value = SCREEN_SPECTRUM;
        break;
    case MENU_DISPLAY_PALETTE:
        if (menu.value > PAL_END - 1)
            menu.value = PAL_END - 1;
        if (menu.value < PAL_DEFAULT)
            menu.value = PAL_DEFAULT;
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
        screenSetBrightness((int8_t)menu.value);
    }
}

static bool menuIsValid(MenuIdx index)
{
    AudioProc *aProc = audioGet();
    Tuner *tuner = tunerGet();

    switch (index) {
    case MENU_NULL:
        // Don't allow null menu
        return false;
    case MENU_AUDIO_IN_0:
    case MENU_AUDIO_IN_1:
    case MENU_AUDIO_IN_2:
    case MENU_AUDIO_IN_3:
    case MENU_AUDIO_IN_4:
    case MENU_AUDIO_IN_5:
    case MENU_AUDIO_IN_6:
    case MENU_AUDIO_IN_7:
        // Limit Audio inputs
        if (index - MENU_AUDIO_IN_0 >= aProc->par.inCnt) {
            return false;
        }
        break;
    case MENU_TUNER_RDS:
        switch (tuner->par.ic) {
        case TUNER_IC_RDA5807:
        case TUNER_IC_SI4703:
        case TUNER_IC_TEST:
            break;
        default:
            return false;
        }
        break;
    case MENU_TUNER_BASS:
        switch (tuner->par.ic) {
        case TUNER_IC_RDA5807:
        case TUNER_IC_TEST:
            break;
        default:
            return false;
        }
        break;
    case MENU_TUNER_VOLUME:
        switch (tuner->par.ic) {
        case TUNER_IC_RDA5807:
        case TUNER_IC_SI4703:
        case TUNER_IC_TEST:
            break;
        default:
            return false;
        }
        break;
    }

    if (tuner->par.ic == TUNER_IC_NO) {
        if (index >= MENU_TUNER_BAND && index <= MENU_TUNER_VOLUME) {
            return false;
        }
    }

    return true;
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
        if ((menuItems[item].parent == menu.parent) && menuIsValid(item)) {
            menu.list[idx++] = (uint8_t)item;
            if (idx >= MENU_MAX_LEN) {
                break;
            }
        }
    }

    menu.listSize = idx;
}

static void menuUpdate(MenuIdx index)
{
    switch (index) {
    case MENU_AUDIO_IC:
        audioReadSettings();
        break;
    case MENU_TUNER_IC:
        tunerReadSettings();
        break;
    default:
        return;
    }

    canvasClear();
    menuSelect(index);
    menu.active = index;
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
            switch (index) {
            case MENU_DISPLAY_BR_STBY:
                screenSetBrightness((int8_t)settingsGet(PARAM_DISPLAY_BR_STBY));
                break;
            case MENU_DISPLAY_BR_WORK:
                screenSetBrightness((int8_t)settingsGet(PARAM_DISPLAY_BR_WORK));
                break;
            }
        } else {
            menuStoreCurrentValue();
            if (index == MENU_DISPLAY_BR_STBY || index == MENU_DISPLAY_BR_WORK) {
                screenSetBrightness((int8_t)settingsGet(PARAM_DISPLAY_BR_WORK));
            }
            menuUpdate(index);
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

__attribute__ ((noinline))
const char *menuGetName(MenuIdx index)
{
    char *name;

    if (index >= MENU_AUDIO_IN_0 && index <= MENU_AUDIO_IN_LAST) {
        name = utilMkStr("%s %d",
                         labelsGet((Label)(LABEL_MENU + MENU_AUDIO_IN)),
                         index - MENU_AUDIO_IN);
    } else if (index >= MENU_RC_DIG_0 && index <= MENU_RC_DIG_9) {
        name = utilMkStr("%s %d",
                         labelsGet((Label)(LABEL_MENU + MENU_RC_DIG)),
                         index - MENU_RC_DIG);
    } else {
        name = utilMkStr("%s",
                         labelsGet((Label)(LABEL_MENU + (index - MENU_NULL))));
    }

    return name;
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
        ret = utilMkStr("%5d", value);
        return ret;
    }

    if (menuItems[index].type == MENU_TYPE_RC) {
        if ((uint16_t)value == EE_NOT_FOUND) {
            ret = noVal;
        } else {
            ret = utilMkStr("%04d", (uint16_t)value);
        }
        return ret;
    }

    // Enum menu types
    switch (index) {
    case MENU_SYSTEM_LANG:
        ret = labelsGetLangName((Lang)value);
        break;
    case MENU_SYSTEM_MUTESTBY:
        ret = labelsGet((Label)(LABEL_MUTESTBY + value));
        break;

    case MENU_I2C_EXT_IN_STAT:
    case MENU_I2C_EXT_BT:
        if (value == I2C_ADDR_DISABLED) {
            ret = labelsGet(LABEL_BOOL_OFF);
        } else {
            ret = utilMkStr("0x%02X", i2cexpGetAddr((uint8_t)value));
        }
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
    case MENU_ALARM_DAYS:
        ret = labelsGet((Label)(LABEL_ALARM_DAY + value));
        break;
    case MENU_SPECTRUM_MODE:
        ret = labelsGet((Label)(LABEL_SPECTRUM_MODE + value));
        break;
    case MENU_DISPLAY_DEF:
        switch ((ScrMode)value) {
        case SCREEN_TIME:
            ret = labelsGet((Label)(LABEL_MENU + MENU_RC_TIME));
            break;
        case SCREEN_AUDIO_INPUT:
            ret = labelsGet((Label)(LABEL_MENU + MENU_AUDIO_IN));
            break;
        default:
            ret = labelsGet((Label)(LABEL_MENU + MENU_SETUP_SPECTRUM));
            break;
        }
        break;
    case MENU_DISPLAY_PALETTE:
        ret = labelsGet((Label)(LABEL_PAL_MODE + value));
        break;
    default:
        ret = noVal;
        break;
    }

    return ret;
}
