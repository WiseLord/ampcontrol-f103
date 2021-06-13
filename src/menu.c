#include "menu.h"

#include <stdio.h>
#include <string.h>

#include "amp.h"
#include "audio/audio.h"
#include "eemul.h"
#include "gui/canvas.h"
#include "i2cexp.h"
#include "input.h"
#include "rc.h"
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

static int16_t menuGetParam(Param param);
static void menuSetParam(Param param, int16_t value);

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
    [MENU_SYSTEM_I2C_EXT]   = {MENU_SETUP_SYSTEM,       MENU_TYPE_PARENT,   PARAM_NULL},
    [MENU_SYSTEM_ENC_RES]   = {MENU_SETUP_SYSTEM,       MENU_TYPE_NUMBER,   PARAM_SYSTEM_ENC_RES},
    [MENU_SYSTEM_SIL_TIM]   = {MENU_SETUP_SYSTEM,       MENU_TYPE_NUMBER,   PARAM_SYSTEM_SIL_TIM},
    [MENU_SYSTEM_RTC_CORR]  = {MENU_SETUP_SYSTEM,       MENU_TYPE_NUMBER,   PARAM_SYSTEM_RTC_CORR},
    [MENU_SYSTEM_STBY_LOW]  = {MENU_SETUP_SYSTEM,       MENU_TYPE_BOOL,     PARAM_SYSTEM_STBY_LOW},
    [MENU_SYSTEM_MUTE_LOW]  = {MENU_SETUP_SYSTEM,       MENU_TYPE_BOOL,     PARAM_SYSTEM_MUTE_LOW},

    [MENU_I2C_EXT_IN_STAT]  = {MENU_SYSTEM_I2C_EXT,     MENU_TYPE_ENUM,     PARAM_I2C_EXT_IN_STAT},
    [MENU_I2C_EXT_GPIO]     = {MENU_SYSTEM_I2C_EXT,     MENU_TYPE_ENUM,     PARAM_I2C_EXT_GPIO},

    [MENU_AUDIO_IC]         = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     PARAM_AUDIO_IC},
    [MENU_AUDIO_SHOWDB]     = {MENU_SETUP_AUDIO,        MENU_TYPE_BOOL,     PARAM_AUDIO_SHOWDB},
    [MENU_AUDIO_MODE]       = {MENU_SETUP_AUDIO,        MENU_TYPE_ENUM,     PARAM_AUDIO_MODE},
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
    [MENU_TUNER_STA_MODE]   = {MENU_SETUP_TUNER,        MENU_TYPE_FLAG,     PARAM_TUNER_FLAGS},
    [MENU_TUNER_FMONO]      = {MENU_SETUP_TUNER,        MENU_TYPE_FLAG,     PARAM_TUNER_FLAGS},
    [MENU_TUNER_RDS]        = {MENU_SETUP_TUNER,        MENU_TYPE_FLAG,     PARAM_TUNER_FLAGS},
    [MENU_TUNER_BASS]       = {MENU_SETUP_TUNER,        MENU_TYPE_FLAG,     PARAM_TUNER_FLAGS},
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
    [MENU_DISPLAY_SCRSAVER] = {MENU_SETUP_DISPLAY,      MENU_TYPE_BOOL,     PARAM_DISPLAY_SCRSAVER},

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

    ret = menuGetParam(menuItems[index].param);

    if (menuItems[index].type == MENU_TYPE_FLAG) {
        switch (index) {
        case MENU_TUNER_STA_MODE:
            ret = !!(ret & TUNER_PARAM_STA_MODE);
            break;
        case MENU_TUNER_FMONO:
            ret = !!(ret & TUNER_PARAM_MONO);
            break;
        case MENU_TUNER_RDS:
            ret = !!(ret & TUNER_PARAM_RDS);
            break;
        case MENU_TUNER_BASS:
            ret = !!(ret & TUNER_PARAM_BASS);
            break;
        }
    }

    return ret;
}

static void paramSetFlag(int16_t *flags, const int16_t flag, bool value)
{
    if (value) {
        *flags |= flag;
    } else {
        *flags &= ~flag;
    }
}

static void menuStoreCurrentValue(void)
{
    MenuIdx index = menu.active;
    int16_t value = menu.value;

    if (index < MENU_END) {
        Param param = menuItems[index].param;

        if (menuItems[index].type == MENU_TYPE_FLAG) {
            int16_t tFlags = tunerGet()->par.flags;

            switch (index) {
            case MENU_TUNER_STA_MODE:
                paramSetFlag(&tFlags, TUNER_PARAM_STA_MODE, value);
                break;
            case MENU_TUNER_FMONO:
                paramSetFlag(&tFlags, TUNER_PARAM_MONO, value);
                break;
            case MENU_TUNER_RDS:
                paramSetFlag(&tFlags, TUNER_PARAM_RDS, value);
                break;
            case MENU_TUNER_BASS:
                paramSetFlag(&tFlags, TUNER_PARAM_BASS, value);
                break;
            }

            value = tFlags;
        }

        menuSetParam(param, value);
        settingsStore(param, value);
    }

    switch (index) {
    case MENU_SYSTEM_STBY_LOW:
        ampPinStby(true);
        break;
    case MENU_SYSTEM_MUTE_LOW:
        ampPinMute(true);
        break;
    case MENU_DISPLAY_BR_STBY:
    case MENU_DISPLAY_BR_WORK:
        ampSetBrightness((int8_t)(value));
        break;
    case MENU_DISPLAY_ROTATE:
        glcdSetOrientation(value ? GLCD_LANDSCAPE_ROT : GLCD_LANDSCAPE);
        canvasClear();
        break;
    case MENU_DISPLAY_PALETTE:
        paletteSetIndex((PalIdx)value);
        canvasGet()->pal = paletteGet();
        break;
    default:
        break;
    }
}

static void menuValueLimit()
{
    switch (menu.active) {
    case MENU_SYSTEM_LANG:
        if (menu.value > LANG_END - 1)
            menu.value = LANG_END - 1;
        if (menu.value < LANG_DEFAULT)
            menu.value = LANG_DEFAULT;
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
    case MENU_I2C_EXT_GPIO:
        if (menu.value > I2C_ADDR_END - 1)
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
    case MENU_AUDIO_MODE:
        if (menu.value > AUDIO_MODE_END - 1)
            menu.value = AUDIO_MODE_END - 1;
        if (menu.value < AUDIO_MODE_2_0)
            menu.value = AUDIO_MODE_2_0;
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
        if (menu.value < IN_DISABLED)
            menu.value = IN_DISABLED;
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

    case MENU_SPECTRUM_MODE:
        if (menu.value > SP_MODE_MIXED)
            menu.value = SP_MODE_MIXED;
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

    default:
        break;
    }

    if (menu.active == MENU_DISPLAY_BR_STBY || menu.active == MENU_DISPLAY_BR_WORK) {
        ampSetBrightness((int8_t)menu.value);
    }
}

static void menuValueChange(int8_t diff)
{
    MenuType type = menuItems[menu.active].type;

    if (type == MENU_TYPE_BOOL || type == MENU_TYPE_FLAG) {
        if (diff)
            menu.value = !menu.value;
        return;
    }

    if (type == MENU_TYPE_RC) {
        RcData rcData = rcRead(false);

        menu.value = (int16_t)(((rcData.addr & 0xFF) << 8) | rcData.cmd);
        return;
    }

    menu.value += diff;

    menuValueLimit();
}

static bool menuIsValid(MenuIdx index)
{
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
        if (index - MENU_AUDIO_IN_0 >= audioGetInputCount()) {
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
        audioReadSettings(AUDIO_IC_TDA7439);
        break;
    case MENU_TUNER_IC:
        tunerReadSettings(TUNER_IC_RDA5807);
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
                ampSetBrightness((int8_t)menuGetParam(PARAM_DISPLAY_BR_STBY));
                break;
            case MENU_DISPLAY_BR_WORK:
                ampSetBrightness((int8_t)menuGetParam(PARAM_DISPLAY_BR_WORK));
                break;
            }
        } else {
            if (index == MENU_AUDIO_MODE) {
                if (!audioIsModeSupported((AudioMode)menu.value)) {
                    menu.value = SP_MODE_STEREO;
                }
            }
            menuStoreCurrentValue();
            if (index == MENU_DISPLAY_BR_STBY || index == MENU_DISPLAY_BR_WORK) {
                ampSetBrightness((int8_t)menuGetParam(PARAM_DISPLAY_BR_WORK));
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

void menuGetName(MenuIdx index, char *str, size_t len)
{
    if (index >= MENU_AUDIO_IN_0 && index <= MENU_AUDIO_IN_LAST) {
        snprintf(str, len, "%s %d",
                 labelsGet((Label)(LABEL_MENU + MENU_AUDIO_IN)),
                 index - MENU_AUDIO_IN);
    } else if (index >= MENU_RC_DIG_0 && index <= MENU_RC_DIG_9) {
        snprintf(str, len, "%s %d",
                 labelsGet((Label)(LABEL_MENU + MENU_RC_DIG)),
                 index - MENU_RC_DIG);
    } else {
        snprintf(str, len, "%s",
                 labelsGet((Label)(LABEL_MENU + (index - MENU_NULL))));
    }
}

void menuGetValueStr(MenuIdx index, char *str, size_t len)
{
    const char *ret = ">";
    static const char *noVal = "--";

    // Parent menu type
    MenuType type = menuItems[index].type;

    if (type == MENU_TYPE_PARENT) {
        snprintf(str, len, "%s", index == MENU_NULL ? "" : ret);
        return;
    }

    int16_t value = menuGetValue(index);

    if (index == menu.active && menu.selected)
        value = menu.value;

    // Bool menu type
    if (type == MENU_TYPE_BOOL || type == MENU_TYPE_FLAG) {
        ret = labelsGet((Label)(LABEL_BOOL_OFF + value));
        snprintf(str, len, "%s", ret);
        return;
    }

    if (type == MENU_TYPE_NUMBER) {
        snprintf(str, len, "%5d", value);
        return;
    }

    if (type == MENU_TYPE_RC) {
        if ((uint16_t)value == EE_NOT_FOUND) {
            snprintf(str, len, "%s", noVal);
            return;
        } else {
            snprintf(str, len, "0x%04X", (uint16_t)value);
            return;
        }
    }

    // Enum menu types
    switch (index) {
    case MENU_SYSTEM_LANG:
        ret = labelsGetLangName((Lang)value);
        break;

    case MENU_I2C_EXT_IN_STAT:
    case MENU_I2C_EXT_GPIO:
        if (value == I2C_ADDR_DISABLED) {
            ret = labelsGet(LABEL_BOOL_OFF);
        } else {
            snprintf(str, len, "0x%02X", i2cExpGetAddr((uint8_t)value));
            return;
        }
        break;

    case MENU_AUDIO_IC:
        ret = labelsGet((Label)(LABEL_AUDIO_IC + value));
        break;
    case MENU_AUDIO_MODE:
        ret = labelsGet((Label)(LABEL_AUDIO_MODE + value));
        break;
    case MENU_AUDIO_IN_0:
    case MENU_AUDIO_IN_1:
    case MENU_AUDIO_IN_2:
    case MENU_AUDIO_IN_3:
    case MENU_AUDIO_IN_4:
    case MENU_AUDIO_IN_5:
    case MENU_AUDIO_IN_6:
    case MENU_AUDIO_IN_7:
        ret = labelsGet((Label)(value == IN_DISABLED ? LABEL_BOOL_OFF : LABEL_IN_TUNER + value));
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
        switch ((ScreenType)value) {
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

    snprintf(str, len, "%s", ret);
}

static int16_t menuGetParam(Param param)
{
    int16_t ret = 0;

    AudioProc *aProc = audioGet();
    Tuner *tuner = tunerGet();
    Spectrum *sp = spGet();
    Alarm *alarm = rtcGetAlarm(0);
    Amp *amp = ampGet();

    switch (param) {

    case PARAM_SYSTEM_LANG:
        ret = labelsGetLang();
        break;
    case PARAM_SYSTEM_ENC_RES:
        ret = inputGet()->encRes;
        break;
    case PARAM_SYSTEM_SIL_TIM:
        ret = amp->silenceTimer;
        break;
    case PARAM_SYSTEM_RTC_CORR:
        ret = rtcGetCorrection();
        break;
    case PARAM_SYSTEM_STBY_LOW:
        ret = amp->stbyLow;
        break;
    case PARAM_SYSTEM_MUTE_LOW:
        ret = amp->muteLow;
        break;

    case PARAM_I2C_EXT_IN_STAT:
        ret = i2cExpGet()->idxInStatus;
        break;
    case PARAM_I2C_EXT_GPIO:
        ret = i2cExpGet()->idxGpio;
        break;

    case PARAM_AUDIO_IC:
        ret = aProc->par.ic;
        break;
    case PARAM_AUDIO_SHOWDB:
        ret = amp->showDb;
        break;
    case PARAM_AUDIO_MODE:
        ret = aProc->par.mode;
        break;

    case PARAM_AUDIO_IN0:
    case PARAM_AUDIO_IN1:
    case PARAM_AUDIO_IN2:
    case PARAM_AUDIO_IN3:
    case PARAM_AUDIO_IN4:
    case PARAM_AUDIO_IN5:
    case PARAM_AUDIO_IN6:
    case PARAM_AUDIO_IN7:
        ret = amp->inType[param - PARAM_AUDIO_IN0];
        break;

    case PARAM_TUNER_IC:
        ret = tuner->par.ic;
        break;
    case PARAM_TUNER_BAND:
        ret = tuner->par.band;
        break;
    case PARAM_TUNER_STEP:
        ret = tuner->par.step;
        break;
    case PARAM_TUNER_DEEMPH:
        ret = tuner->par.deemph;
        break;
    case PARAM_TUNER_FLAGS:
        ret = tuner->par.flags;
        break;
    case PARAM_TUNER_VOLUME:
        ret = tuner->par.volume;
        break;

    case PARAM_ALARM_HOUR:
        ret = alarm->hour;
        break;
    case PARAM_ALARM_MINUTE:
        ret = alarm->min;
        break;
    case PARAM_ALARM_DAYS:
        ret = alarm->days;
        break;

    case PARAM_SPECTRUM_MODE:
        ret = sp->mode;
        break;
    case PARAM_SPECTRUM_PEAKS:
        ret = ((sp->flags & SP_FLAG_PEAKS) == SP_FLAG_PEAKS);
        break;
    case PARAM_SPECTRUM_GRAD:
        ret = ((sp->flags & SP_FLAG_GRAD) == SP_FLAG_GRAD);
        break;

    case PARAM_DISPLAY_BR_STBY:
        ret = amp->brStby;
        break;
    case PARAM_DISPLAY_BR_WORK:
        ret = amp->brWork;
        break;
    case PARAM_DISPLAY_ROTATE:
        ret = (glcdGet()->orientation == GLCD_LANDSCAPE_ROT);
        break;
    case PARAM_DISPLAY_DEF:
        ret = amp->defScreen;
        break;
    case PARAM_DISPLAY_PALETTE:
        ret = paletteGetIndex();
        break;
    case PARAM_DISPLAY_SCRSAVER:
        ret = amp->scrSaver;
        break;

    default:
        break;
    }

    if (param >= PARAM_RC_STBY_SWITCH && param < PARAM_RC_STBY_SWITCH + RC_CMD_END) {
        ret = (int16_t)rcGetCode(param - PARAM_RC_STBY_SWITCH);
    }

    return  ret;
}

static void menuSetParam(Param param, int16_t value)
{
    AudioProc *aProc = audioGet();
    Tuner *tuner = tunerGet();
    Spectrum *sp = spGet();
    Alarm *alarm = rtcGetAlarm(0);
    Amp *amp = ampGet();

    switch (param) {

    case PARAM_SYSTEM_LANG:
        labelsSetLang((Lang)value);
        break;
    case PARAM_SYSTEM_ENC_RES:
        inputGet()->encRes = (int8_t)value;
        break;
    case PARAM_SYSTEM_SIL_TIM:
        amp->silenceTimer = (uint8_t)value;
        break;
    case PARAM_SYSTEM_RTC_CORR:
        rtcSetCorrection(value);
        break;
    case PARAM_SYSTEM_STBY_LOW:
        amp->stbyLow = (bool)value;
        break;
    case PARAM_SYSTEM_MUTE_LOW:
        amp->muteLow = (bool)value;
        break;

    case PARAM_I2C_EXT_IN_STAT:
        i2cExpGet()->idxInStatus = (I2cAddrIdx)value;
        break;
    case PARAM_I2C_EXT_GPIO:
        i2cExpGet()->idxGpio = (I2cAddrIdx)value;
        break;

    case PARAM_AUDIO_IC:
        aProc->par.ic = (AudioIC)(value);
        break;
    case PARAM_AUDIO_SHOWDB:
        amp->showDb = (bool)value;
        break;
    case PARAM_AUDIO_MODE:
        aProc->par.mode = (AudioMode)value;
        break;
    case PARAM_AUDIO_IN0:
    case PARAM_AUDIO_IN1:
    case PARAM_AUDIO_IN2:
    case PARAM_AUDIO_IN3:
    case PARAM_AUDIO_IN4:
    case PARAM_AUDIO_IN5:
    case PARAM_AUDIO_IN6:
    case PARAM_AUDIO_IN7:
        amp->inType[param - PARAM_AUDIO_IN0] = (InputType)value;
        break;

    case PARAM_TUNER_IC:
        tuner->par.ic = (TunerIC)value;
        break;
    case PARAM_TUNER_BAND:
        tuner->par.band = (TunerBand)value;
        break;
    case PARAM_TUNER_STEP:
        tuner->par.step = (TunerStep)value;
        break;
    case PARAM_TUNER_DEEMPH:
        tuner->par.deemph = (TunerDeemph)value;
        break;
    case PARAM_TUNER_FLAGS:
        tuner->par.flags = (TunerParamFlag)value;
        break;
    case PARAM_TUNER_VOLUME:
        tuner->par.volume = (int8_t)value;
        break;

    case PARAM_ALARM_HOUR:
        alarm->hour = (int8_t)value;
        break;
    case PARAM_ALARM_MINUTE:
        alarm->min = (int8_t)value;
        break;
    case PARAM_ALARM_DAYS:
        alarm->days = (AlarmDay)value;
        break;

    case PARAM_SPECTRUM_MODE:
        sp->mode = (SpMode)value;
        break;
    case PARAM_SPECTRUM_PEAKS:
        sp->flags = (value ? (sp->flags | SP_FLAG_PEAKS) : (sp->flags & ~SP_FLAG_PEAKS));
        break;
    case PARAM_SPECTRUM_GRAD:
        sp->flags = (value ? (sp->flags | SP_FLAG_GRAD) : (sp->flags & ~SP_FLAG_GRAD));
        break;

    case PARAM_DISPLAY_BR_STBY:
        amp->brStby = (int8_t)value;
        break;
    case PARAM_DISPLAY_BR_WORK:
        amp->brWork = (int8_t)value;
        break;
    case PARAM_DISPLAY_ROTATE:
        glcdGet()->orientation = (value ? GLCD_LANDSCAPE_ROT : GLCD_LANDSCAPE);
        break;
    case PARAM_DISPLAY_DEF:
        amp->defScreen = (ScreenType)value;
        break;
    case PARAM_DISPLAY_PALETTE:
        paletteSetIndex((PalIdx)value);
        break;
    case PARAM_DISPLAY_SCRSAVER:
        amp->scrSaver = (bool)value;
        break;

    default:
        break;
    }

    if (param >= PARAM_RC_STBY_SWITCH && param < PARAM_RC_STBY_SWITCH + RC_CMD_END) {
        rcSaveCode((uint16_t)(param - PARAM_RC_STBY_SWITCH), (uint16_t)value);
    }
}
