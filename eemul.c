#include "eemul.h"

#include <stm32f103xb.h>
#include <stm32f1xx.h>
#include <stm32f1xx_ll_utils.h>

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

static inline void eeUnlock(void) __attribute__((always_inline));
static inline void eeUnlock(void)
{
    WRITE_REG(FLASH->KEYR, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR, FLASH_KEY2);
}

static inline void eeLock(void) __attribute__((always_inline));
static inline void eeLock(void)
{
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);
}

static inline void eeWaitBusy(void) __attribute__((always_inline));
static inline void eeWaitBusy(void)
{
    // TODO: add some timeout
    while (READ_BIT(FLASH->SR, FLASH_SR_BSY));
}

static uint16_t eeFindEmptyCell(void)
{
    // TODO: implement binary search here

    for (int16_t i = EE_DATA_OFT; i < EE_CAPACITY; i++) {
        if (*ADDR(i) == EE_EMPTY)
            return i;
    }

    return EE_EMPTY;
}

static uint16_t eeFindLastCell(uint16_t addr, uint16_t pos)
{
    for (int16_t i = pos; i >= EE_DATA_OFT; i--) {
        if (*ADDR(i) == addr)
            return i;
    }

    return EE_EMPTY;
}

static void eeCopyPage(uint16_t addr, uint16_t data)
{
    uint16_t *raw = ADDR_CP(EE_DATA_OFT);

    eeUnlock();
    SET_BIT(FLASH->CR, FLASH_CR_PG);

    for (uint16_t param = 0; param < EE_PARAM_END; param++) {
        uint16_t last = eeFindLastCell(param, EE_CAPACITY - 1);
        if (last != EE_EMPTY) {
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

    if (cell != EE_EMPTY) {
        uint16_t last = eeFindLastCell(eeAddr, cell - 1);
        if (last == EE_EMPTY || *DATA(last) != eeData) {
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
        if (last == EE_EMPTY || *DATA(last) != eeData) {
            eeCopyPage(eeAddr, eeData);
            eeSwapPage();
        }
    }
}

uint16_t eeRead(EE_Param param)
{
    uint16_t eeAddr = (uint16_t)param;

    uint16_t cell = eeFindEmptyCell();
    if (cell != EE_EMPTY) {
        uint16_t last = eeFindLastCell(eeAddr, cell - 1);
        if (last == EE_EMPTY) {
            return EE_EMPTY;
        }
        return *DATA(last);
    }
    return EE_EMPTY;
}
