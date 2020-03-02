#include "settings.h"

#include "audio/audiodefs.h"
#include "display/glcd.h"
#include "eemul.h"
#include "i2cexp.h"
#include "input.h"
#include "rc.h"
#include "screen.h"
#include "spectrum.h"
#include "tr/labels.h"
#include "tuner/tunerdefs.h"

static uint8_t silenceTimer = 0;
static int16_t rtcCorr = 0;
static uint8_t muteStby = 0;
static I2cAddrIdx i2cExtInIdx = I2C_ADDR_DISABLED;
static I2cAddrIdx i2cBtIdx = I2C_ADDR_DISABLED;
static int8_t brStby = 3;
static int8_t brWork = LCD_BR_MAX;

#define GENERATE_EE_RC_MAP(CMD)  [PARAM_RC_ ## CMD] = {0x80 + RC_CMD_ ## CMD, (int16_t)EE_NOT_FOUND},

static const EE_Map eeMap[] = {
    [PARAM_NULL]            =   {0x00,  0},

    [PARAM_AUDIO_IC]        =   {0x01,  AUDIO_IC_TEST},
    [PARAM_AUDIO_INPUT]     =   {0x02,  0},
    [PARAM_AUDIO_LOUDNESS]  =   {0x03,  false},
    [PARAM_AUDIO_SURROUND]  =   {0x04,  false},
    [PARAM_AUDIO_EFFECT3D]  =   {0x05,  false},
    [PARAM_AUDIO_BYPASS]    =   {0x06,  false},

    [PARAM_AUDIO_IN0]       =   {0x10,  0},
    [PARAM_AUDIO_IN1]       =   {0x11,  1},
    [PARAM_AUDIO_IN2]       =   {0x12,  2},
    [PARAM_AUDIO_IN3]       =   {0x13,  3},
    [PARAM_AUDIO_IN4]       =   {0x14,  4},
    [PARAM_AUDIO_IN5]       =   {0x15,  5},
    [PARAM_AUDIO_IN6]       =   {0x16,  6},
    [PARAM_AUDIO_IN7]       =   {0x17,  7},

    [PARAM_AUDIO_GAIN0]     =   {0x18,  0},
    [PARAM_AUDIO_GAIN1]     =   {0x19,  0},
    [PARAM_AUDIO_GAIN2]     =   {0x1A,  0},
    [PARAM_AUDIO_GAIN3]     =   {0x1B,  0},
    [PARAM_AUDIO_GAIN4]     =   {0x1C,  0},
    [PARAM_AUDIO_GAIN5]     =   {0x1D,  0},
    [PARAM_AUDIO_GAIN6]     =   {0x1E,  0},
    [PARAM_AUDIO_GAIN7]     =   {0x1F,  0},

    [PARAM_AUDIO_VOLUME]    =   {0x20,  0},
    [PARAM_AUDIO_BASS]      =   {0x21,  0},
    [PARAM_AUDIO_MIDDLE]    =   {0x22,  0},
    [PARAM_AUDIO_TREBLE]    =   {0x23,  0},
    [PARAM_AUDIO_FRONTREAR] =   {0x24,  0},
    [PARAM_AUDIO_BALANCE]   =   {0x25,  0},
    [PARAM_AUDIO_CENTER]    =   {0x26,  0},
    [PARAM_AUDIO_SUBWOOFER] =   {0x27,  0},
    [PARAM_AUDIO_PREAMP]    =   {0x28,  0},

    [PARAM_TUNER_IC]        =   {0x30,  TUNER_IC_TEST},
    [PARAM_TUNER_BAND]      =   {0x31,  TUNER_BAND_FM_US_EUROPE},
    [PARAM_TUNER_STEP]      =   {0x32,  TUNER_STEP_100K},
    [PARAM_TUNER_DEEMPH]    =   {0x33,  TUNER_DEEMPH_50u},
    [PARAM_TUNER_STA_MODE]  =   {0x34,  true},
    [PARAM_TUNER_FMONO]     =   {0x35,  false},
    [PARAM_TUNER_RDS]       =   {0x36,  true},
    [PARAM_TUNER_BASS]      =   {0x37,  false},
    [PARAM_TUNER_VOLUME]    =   {0x38,  TUNER_VOLUME_MAX},
    [PARAM_TUNER_FREQ]      =   {0x39,  9950},

    [PARAM_DISPLAY_BR_STBY] =   {0x40,  3},
    [PARAM_DISPLAY_BR_WORK] =   {0x41,  LCD_BR_MAX},
    [PARAM_DISPLAY_ROTATE]  =   {0x42,  false},
    [PARAM_DISPLAY_DEF]     =   {0x43,  SCREEN_AUDIO_INPUT},
    [PARAM_DISPLAY_PALETTE] =   {0x44,  PAL_DEFAULT},

    [PARAM_SPECTRUM_MODE]   =   {0x50,  SP_MODE_STEREO},
    [PARAM_SPECTRUM_PEAKS]  =   {0x51,  true},
    [PARAM_SPECTRUM_GRAD]   =   {0x52,  false},

    [PARAM_ALARM_HOUR]      =   {0x60,  7},
    [PARAM_ALARM_MINUTE]    =   {0x61,  30},
    [PARAM_ALARM_DAYS]      =   {0x62,  0},

    [PARAM_SYSTEM_LANG]     =   {0x70,  LANG_DEFAULT},
    [PARAM_SYSTEM_MUTESTBY] =   {0x71,  MUTESTBY_POS},
    [PARAM_SYSTEM_SIL_TIM]  =   {0x72,  5},
    [PARAM_SYSTEM_RTC_CORR] =   {0x73,  0},
    [PARAM_SYSTEM_ENC_RES]  =   {0x74,  4},

    [PARAM_I2C_EXT_IN_STAT] =   {0x78,  I2C_ADDR_DISABLED},
    [PARAM_I2C_EXT_BT]      =   {0x79,  I2C_ADDR_DISABLED},

    FOREACH_CMD(GENERATE_EE_RC_MAP)
};

void settingsInit(void)
{
    eeInit();

    for (Param par = PARAM_NULL + 1; par < PARAM_END; par++) {
        settingsSet(par, settingsRead(par));
    }
}

int16_t settingsGet(Param param)
{
    int16_t ret = 0;

    AudioProc *aProc = audioGet();
    Tuner *tuner = tunerGet();
    Spectrum *sp = spGet();
    Alarm *alarm = rtcGetAlarm(0);

    switch (param) {

    case PARAM_AUDIO_IC:
        ret = aProc->par.ic;
        break;
    case PARAM_AUDIO_INPUT:
        ret = aProc->par.input;
        break;
    case PARAM_AUDIO_LOUDNESS:
        ret = aProc->par.loudness;
        break;
    case PARAM_AUDIO_SURROUND:
        ret = aProc->par.surround;
        break;
    case PARAM_AUDIO_EFFECT3D:
        ret = aProc->par.effect3d;
        break;
    case PARAM_AUDIO_BYPASS:
        ret = aProc->par.bypass;
        break;

    case PARAM_AUDIO_IN0:
    case PARAM_AUDIO_IN1:
    case PARAM_AUDIO_IN2:
    case PARAM_AUDIO_IN3:
    case PARAM_AUDIO_IN4:
    case PARAM_AUDIO_IN5:
    case PARAM_AUDIO_IN6:
    case PARAM_AUDIO_IN7:
        ret = aProc->par.inType[param - PARAM_AUDIO_IN0];
        break;

    case PARAM_AUDIO_GAIN0:
    case PARAM_AUDIO_GAIN1:
    case PARAM_AUDIO_GAIN2:
    case PARAM_AUDIO_GAIN3:
    case PARAM_AUDIO_GAIN4:
    case PARAM_AUDIO_GAIN5:
    case PARAM_AUDIO_GAIN6:
    case PARAM_AUDIO_GAIN7:
        ret = aProc->par.gain[param - PARAM_AUDIO_GAIN0];
        break;

    case PARAM_AUDIO_VOLUME:
    case PARAM_AUDIO_BASS:
    case PARAM_AUDIO_MIDDLE:
    case PARAM_AUDIO_TREBLE:
    case PARAM_AUDIO_FRONTREAR:
    case PARAM_AUDIO_BALANCE:
    case PARAM_AUDIO_CENTER:
    case PARAM_AUDIO_SUBWOOFER:
    case PARAM_AUDIO_PREAMP:
        ret = aProc->par.tune[param - PARAM_AUDIO_VOLUME].value;
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
    case PARAM_TUNER_STA_MODE:
        ret = tuner->par.stationMode;
        break;
    case PARAM_TUNER_FMONO:
        ret = tuner->par.forcedMono;
        break;
    case PARAM_TUNER_RDS:
        ret = tuner->par.rds;
        break;
    case PARAM_TUNER_BASS:
        ret = tuner->par.bassBoost;
        break;
    case PARAM_TUNER_VOLUME:
        ret = tuner->par.volume;
        break;
    case PARAM_TUNER_FREQ:
        ret = (int16_t)tuner->status.freq;
        break;

    case PARAM_DISPLAY_BR_STBY:
        ret = brStby;
        break;
    case PARAM_DISPLAY_BR_WORK:
        ret = brWork;
        break;
    case PARAM_DISPLAY_ROTATE:
        ret = glcdGet()->rotate;
        break;
    case PARAM_DISPLAY_DEF:
        ret = screenGet()->def;
        break;
    case PARAM_DISPLAY_PALETTE:
        ret = paletteGetIndex();
        break;

    case PARAM_SPECTRUM_MODE:
        ret = sp->mode;
        break;
    case PARAM_SPECTRUM_PEAKS:
        ret = sp->peaks;
        break;
    case PARAM_SPECTRUM_GRAD:
        ret = sp->grad;
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

    case PARAM_SYSTEM_LANG:
        ret = labelsGetLang();
        break;
    case PARAM_SYSTEM_MUTESTBY:
        ret = muteStby;
        break;
    case PARAM_SYSTEM_ENC_RES:
        ret = inputGetEncRes();
        break;
    case PARAM_SYSTEM_SIL_TIM:
        ret = silenceTimer;
        break;
    case PARAM_SYSTEM_RTC_CORR:
        ret = rtcCorr;
        break;

    case PARAM_I2C_EXT_IN_STAT:
        ret = i2cExtInIdx;
        break;
    case PARAM_I2C_EXT_BT:
        ret = i2cBtIdx;
        break;

    default:
        break;
    }

    if (param >= PARAM_RC_STBY_SWITCH && param < PARAM_RC_STBY_SWITCH + RC_CMD_END) {
        ret = (int16_t)rcGetCode(param - PARAM_RC_STBY_SWITCH);
    }

    return  ret;
}

void settingsSet(Param param, int16_t value)
{
    AudioProc *aProc = audioGet();
    Tuner *tuner = tunerGet();
    Spectrum *sp = spGet();
    Alarm *alarm = rtcGetAlarm(0);

    switch (param) {

    case PARAM_AUDIO_IC:
        aProc->par.ic = (AudioIC)(value);
        break;
    case PARAM_AUDIO_INPUT:
        aProc->par.input = (int8_t)value;
        break;
    case PARAM_AUDIO_LOUDNESS:
        aProc->par.loudness = (int8_t)value;
        break;
    case PARAM_AUDIO_SURROUND:
        aProc->par.surround = (int8_t)value;
        break;
    case PARAM_AUDIO_EFFECT3D:
        aProc->par.effect3d = (int8_t)value;
        break;
    case PARAM_AUDIO_BYPASS:
        aProc->par.bypass = (int8_t)value;
        break;

    case PARAM_AUDIO_IN0:
    case PARAM_AUDIO_IN1:
    case PARAM_AUDIO_IN2:
    case PARAM_AUDIO_IN3:
    case PARAM_AUDIO_IN4:
    case PARAM_AUDIO_IN5:
    case PARAM_AUDIO_IN6:
    case PARAM_AUDIO_IN7:
        aProc->par.inType[param - PARAM_AUDIO_IN0] = (InputType)value;
        break;

    case PARAM_AUDIO_GAIN0:
    case PARAM_AUDIO_GAIN1:
    case PARAM_AUDIO_GAIN2:
    case PARAM_AUDIO_GAIN3:
    case PARAM_AUDIO_GAIN4:
    case PARAM_AUDIO_GAIN5:
    case PARAM_AUDIO_GAIN6:
    case PARAM_AUDIO_GAIN7:
        aProc->par.gain[param - PARAM_AUDIO_GAIN0] = (int8_t)value;
        break;

    case PARAM_AUDIO_VOLUME:
    case PARAM_AUDIO_BASS:
    case PARAM_AUDIO_MIDDLE:
    case PARAM_AUDIO_TREBLE:
    case PARAM_AUDIO_FRONTREAR:
    case PARAM_AUDIO_BALANCE:
    case PARAM_AUDIO_CENTER:
    case PARAM_AUDIO_SUBWOOFER:
    case PARAM_AUDIO_PREAMP:
        aProc->par.tune[param - PARAM_AUDIO_VOLUME].value = (int8_t)value;
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
    case PARAM_TUNER_STA_MODE:
        tuner->par.stationMode = (bool)value;
        break;
    case PARAM_TUNER_FMONO:
        tuner->par.forcedMono = (bool)value;
        break;
    case PARAM_TUNER_RDS:
        tuner->par.rds = (bool)value;
        break;
    case PARAM_TUNER_BASS:
        tuner->par.bassBoost = (bool)value;
        break;
    case PARAM_TUNER_VOLUME:
        tuner->par.volume = (int8_t)value;
        break;
    case PARAM_TUNER_FREQ:
        tuner->status.freq = (uint16_t)value;
        break;

    case PARAM_DISPLAY_BR_STBY:
        brStby = (int8_t)value;
        break;
    case PARAM_DISPLAY_BR_WORK:
        brWork = (int8_t)value;
        break;
    case PARAM_DISPLAY_ROTATE:
        glcdGet()->rotate = (bool)value;
        break;
    case PARAM_DISPLAY_DEF:
        screenGet()->def = (ScrMode)value;
        break;
    case PARAM_DISPLAY_PALETTE:
        paletteSetIndex((PalIdx)value);
        break;

    case PARAM_SPECTRUM_MODE:
        sp->mode = (SpMode)value;
        break;
    case PARAM_SPECTRUM_PEAKS:
        sp->peaks = (bool)value;
        break;
    case PARAM_SPECTRUM_GRAD:
        sp->grad = (bool)value;
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

    case PARAM_SYSTEM_LANG:
        labelsSetLang((Lang)value);
        break;
    case PARAM_SYSTEM_MUTESTBY:
        muteStby = (uint8_t)value;
        break;
    case PARAM_SYSTEM_ENC_RES:
        inputSetEncRes((int8_t)value);
        break;
    case PARAM_SYSTEM_SIL_TIM:
        silenceTimer = (uint8_t)value;
        break;
    case PARAM_SYSTEM_RTC_CORR:
        rtcCorr = value;
        break;

    case PARAM_I2C_EXT_IN_STAT:
        i2cExtInIdx = (I2cAddrIdx)value;
        break;
    case PARAM_I2C_EXT_BT:
        i2cBtIdx = (I2cAddrIdx)value;
        break;

    default:
        break;
    }

    if (param >= PARAM_RC_STBY_SWITCH && param < PARAM_RC_STBY_SWITCH + RC_CMD_END) {
        rcSaveCode((uint16_t)(param - PARAM_RC_STBY_SWITCH), (uint16_t)value);
    }
}

int16_t settingsRead(Param param)
{
    if (param == PARAM_NULL || param >= PARAM_END) {
        return (int16_t)EE_NOT_FOUND;
    }

    uint16_t value = eeReadRaw(eeMap[param].cell);

    if (value != EE_NOT_FOUND) {
        return (int16_t)value;
    }

    // Return default value if not found
    return eeMap[param].def;
}

void settingsStore(Param param, int16_t value)
{
    if (param == PARAM_NULL || param >= PARAM_END) {
        return;
    }

    eeUpdateRaw(eeMap[param].cell, (uint16_t)value);
}

const EE_Map *eeMapGet(void)
{
    return eeMap;
}
