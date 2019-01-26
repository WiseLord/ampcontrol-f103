#include "eemul.h"

#include <stm32f103xb.h>
#include <stm32f1xx.h>
#include <stm32f1xx_ll_utils.h>

#include "audio/audiodefs.h"
#include "display/dispdefs.h"
#include "tr/labels.h"
#include "tuner/tunerdefs.h"
#include "spectrum.h"

#define GENERATE_EE_RC_MAP(CMD)  {EE_RC_ ## CMD, (int16_t)EE_NOT_FOUND},
#define EEMAP_SIZE      (sizeof(eeMap) / sizeof(eeMap[0]))

#define EE_DATA_OFT     1 // Cell 0 is used for the header

#define EE_CAPACITY     (EE_PAGE_SIZE / sizeof(uint32_t) * EE_PAGE_STEP)

#define HEAD_ERASED     (uint32_t)0xFFFF
#define HEAD_VALID      (uint32_t)0x0000

#define OFT(x)          (FLASH_BASE + EE_PAGE_SIZE * currPage + 4 * x)
#define ADDR(x)         (uint16_t *)(OFT(x))
#define DATA(x)         (uint16_t *)(OFT(x) + 2)

#define OFT_CP(x)       (FLASH_BASE + EE_PAGE_SIZE * (currPage + EE_PAGE_STEP >= EE_PAGE_END ? EE_PAGE_0 : currPage + EE_PAGE_STEP) + 4 * x)
#define ADDR_CP(x)      (uint16_t *)(OFT_CP(x))

static uint16_t currPage;

typedef struct {
    uint16_t par; // EE_Param
    int16_t def; // Default value
} EE_Map;

static const EE_Map eeMap[] = {
    {EE_NULL,                   0},

    {EE_AUDIO_IC,               AUDIO_IC_TDA7439},
    {EE_AUDIO_INPUT,            0},
    {EE_AUDIO_LOUDNESS,         false},
    {EE_AUDIO_SURROUND,         false},
    {EE_AUDIO_EFFECT3D,         false},
    {EE_AUDIO_BYPASS,           false},

    {EE_AUDIO_IN0,              0},
    {EE_AUDIO_IN1,              1},
    {EE_AUDIO_IN2,              2},
    {EE_AUDIO_IN3,              3},
    {EE_AUDIO_IN4,              4},
    {EE_AUDIO_IN5,              5},
    {EE_AUDIO_IN6,              6},
    {EE_AUDIO_IN7,              7},

    {EE_AUDIO_GAIN0,            0},
    {EE_AUDIO_GAIN1,            0},
    {EE_AUDIO_GAIN2,            0},
    {EE_AUDIO_GAIN3,            0},
    {EE_AUDIO_GAIN4,            0},
    {EE_AUDIO_GAIN5,            0},
    {EE_AUDIO_GAIN6,            0},
    {EE_AUDIO_GAIN7,            0},

    {EE_AUDIO_PARAM_VOLUME,     0},
    {EE_AUDIO_PARAM_BASS,       0},
    {EE_AUDIO_PARAM_MIDDLE,     0},
    {EE_AUDIO_PARAM_TREBLE,     0},
    {EE_AUDIO_PARAM_FRONTREAR,  0},
    {EE_AUDIO_PARAM_BALANCE,    0},
    {EE_AUDIO_PARAM_CENTER,     0},
    {EE_AUDIO_PARAM_SUBWOOFER,  0},
    {EE_AUDIO_PARAM_PREAMP,     0},

    {EE_TUNER_IC,               TUNER_IC_RDA5807},
    {EE_TUNER_BAND,             TUNER_BAND_FM_US_EUROPE},
    {EE_TUNER_STEP,             TUNER_STEP_100K},
    {EE_TUNER_DEEMPH,           TUNER_DEEMPH_50u},
    {EE_TUNER_MODE,             TUNER_MODE_GRID},
    {EE_TUNER_FMONO,            false},
    {EE_TUNER_RDS,              true},
    {EE_TUNER_BASS,             false},
    {EE_TUNER_VOLUME,           TUNER_VOLUME_MAX},
    {EE_TUNER_FREQ,             9950},

    {EE_DISPLAY_BR_STBY,        3},
    {EE_DISPLAY_BR_WORK,        LCD_BR_MAX},
    {EE_DISPLAY_ROTATE,         false},

    {EE_SPECTRUM_MODE,          SP_MODE_STEREO},
    {EE_INPUT_ENC_RES,          4},

    {EE_LANGUAGE,               LANG_DEFAULT},

    FOREACH_CMD(GENERATE_EE_RC_MAP)
};

__attribute__((always_inline))
static inline void eeUnlock(void)
{
    WRITE_REG(FLASH->KEYR, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR, FLASH_KEY2);
}

__attribute__((always_inline))
static inline void eeLock(void)
{
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);
}

__attribute__((always_inline))
static inline void eeWaitBusy(void)
{
    // TODO: add some timeout
    while (READ_BIT(FLASH->SR, FLASH_SR_BSY));
}

static uint16_t eeFindEmptyCell(void)
{
    uint16_t left = EE_DATA_OFT;
    uint16_t right = EE_CAPACITY;

    while (left != right) {
        uint16_t mid = (left + right) / 2;

        if (*ADDR(mid) == EE_EMPTY) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }

    if (left < EE_CAPACITY) {
        return left;
    }

    return EE_NOT_FOUND;
}

static uint16_t eeFindLastCell(uint16_t addr, uint16_t pos)
{
    for (uint16_t i = pos; i >= EE_DATA_OFT; i--) {
        if (*ADDR(i) == addr)
            return i;
    }

    return EE_NOT_FOUND;
}

static void eeCopyPage(uint16_t addr, uint16_t data)
{
    uint16_t *raw = ADDR_CP(EE_DATA_OFT);

    eeUnlock();
    SET_BIT(FLASH->CR, FLASH_CR_PG);

    for (uint16_t i = 0; i < EEMAP_SIZE; i++) {
        uint16_t param = eeMap[i].par;
        uint16_t last = eeFindLastCell(param, EE_CAPACITY - 1);
        if (last != EE_NOT_FOUND) {
            uint16_t eeAddr = *ADDR(last);
            uint16_t eeData = *DATA(last);

            if (eeAddr == addr)
                eeData = data;

            *raw = eeAddr;
            eeWaitBusy();
            raw = (uint16_t *)((uint32_t)raw + 2);
            *raw = eeData;
            eeWaitBusy();
            raw = (uint16_t *)((uint32_t)raw + 2);
        }
    }

    CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
    eeLock();
}

static void eeFormatPage(uint16_t page)
{
    uint16_t *raw;

    eeUnlock();
    eeWaitBusy();
    SET_BIT(FLASH->CR, FLASH_CR_PG);
    raw = (uint16_t *)(FLASH_BASE + EE_PAGE_SIZE * page);
    *raw = HEAD_VALID & 0xFFFF;
    eeWaitBusy();
    CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
    eeLock();
}

static void eeErasePage(uint16_t page)
{
    eeUnlock();
    eeWaitBusy();
    SET_BIT(FLASH->CR, FLASH_CR_PER);
    for (uint8_t i = 0; i < EE_PAGE_STEP; i++) {
        WRITE_REG(FLASH->AR, FLASH_BASE + EE_PAGE_SIZE * page + i);
        SET_BIT(FLASH->CR, FLASH_CR_STRT);
        eeWaitBusy();
    }
    CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
    eeLock();
}

static void eeSwapPage(void)
{
    eeErasePage(currPage);
    currPage += EE_PAGE_STEP;
    if (currPage >= EE_PAGE_END) {
        currPage = EE_PAGE_0;
    }
    eeFormatPage(currPage);
}

static uint16_t eeReadRaw(EE_Param param)
{
    uint16_t eeAddr = (uint16_t)param;
    uint16_t ret = EE_NOT_FOUND;

    uint16_t cell = eeFindEmptyCell();
    if (cell != EE_NOT_FOUND) {
        uint16_t last = eeFindLastCell(eeAddr, cell - 1);
        if (last == EE_NOT_FOUND) {
            ret = EE_NOT_FOUND;
        } else {
            ret = *DATA(last);
        }
    }

    return ret;
}

void eeInit()
{
    LL_mDelay(1);

    uint16_t head0 = *(uint16_t *)(FLASH_BASE + EE_PAGE_SIZE * EE_PAGE_0);
    uint16_t head1 = *(uint16_t *)(FLASH_BASE + EE_PAGE_SIZE * EE_PAGE_1);

    currPage = EE_PAGE_0;

    if ((head0 != HEAD_VALID && head0 != HEAD_VALID) || (head0 == head1)) {
        eeErasePage(EE_PAGE_0);
        eeErasePage(EE_PAGE_1);
        eeFormatPage(EE_PAGE_0);
    } else if (HEAD_VALID == head0) {
        currPage = EE_PAGE_0;
    } else if (HEAD_VALID == head1) {
        currPage = EE_PAGE_1;
    }
}

void eeUpdate(EE_Param param, int16_t data)
{
    uint16_t *raw;
    uint16_t eeAddr = (uint16_t)param;
    uint16_t eeData = (uint16_t)data;

    uint16_t cell = eeFindEmptyCell();

    if (cell != EE_NOT_FOUND) {
        uint16_t last = eeFindLastCell(eeAddr, cell - 1);
        if (last == EE_NOT_FOUND || *DATA(last) != eeData) {
            eeUnlock();
            eeWaitBusy();
            SET_BIT(FLASH->CR, FLASH_CR_PG);
            raw = ADDR(cell);
            *raw = eeAddr;
            eeWaitBusy();
            raw = DATA(cell);
            *raw = eeData;
            eeWaitBusy();
            CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
            eeLock();
        }
    } else {
        uint16_t last = eeFindLastCell(eeAddr, EE_CAPACITY - 1);
        if (last == EE_NOT_FOUND || *DATA(last) != eeData) {
            eeCopyPage(eeAddr, eeData);
            eeSwapPage();
        }
    }
}

int16_t eeRead(EE_Param param)
{
    int16_t eeData = (int16_t)eeReadRaw(param);
    int16_t def = (int16_t)EE_NOT_FOUND;

    for (uint16_t i = 0; i < EEMAP_SIZE; i++) {
        if (eeMap[i].par == param) {
            def = eeMap[i].def;
            break;
        }
    }

    return (eeData == (int16_t)EE_NOT_FOUND ? def : eeData);
}
