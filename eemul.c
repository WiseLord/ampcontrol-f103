#include "eemul.h"

#include "hwlibs.h"

#include "settings.h"

#define REC_SIZE            4
#define RECS_ON_PAGE        (EE_PAGE_SIZE / REC_SIZE)

#define EE_PAGE_STEP        (EE_PAGE_1 - EE_PAGE_0)

#define EE_CELLS_NUM        (RECS_ON_PAGE * (EE_PAGE_STEP))

#define PAGE_ADDR(page)     (FLASH_BASE + EE_PAGE_SIZE * (page))

static uint16_t currPage;
#define copyPage            ((currPage == EE_PAGE_0) ? EE_PAGE_1 : EE_PAGE_0)

#define OFT(x)              (PAGE_ADDR(currPage) + REC_SIZE * x)
#define ADDR(x)             (uint16_t *)(OFT(x))
#define DATA(x)             (uint16_t *)(OFT(x) + 2)

#define OFT_CP(x)           (PAGE_ADDR(copyPage) + REC_SIZE * x)
#define ADDR_CP(x)          ((uint16_t *)(OFT_CP(x)))
#define DATA_CP(x)          ((uint16_t *)(OFT_CP(x) + 2))

#define EE_DATA_OFT         1 // Cell 0 is used for the header

#define HEAD_ERASED         ((uint32_t)0xFFFF)
#define HEAD_VALID          ((uint32_t)0x0000)

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
    while (READ_BIT(FLASH->SR, FLASH_SR_BSY));
}

static uint16_t eeFindEmptyCell(void)
{
    uint16_t left = EE_DATA_OFT;
    uint16_t right = EE_CELLS_NUM;

    while (left != right) {
        uint16_t mid = (left + right) / 2;

        if (*ADDR(mid) == EE_EMPTY) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }

    if (left < EE_CELLS_NUM) {
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

    const EE_Map *eeMap = eeMapGet();

    for (uint16_t i = 0; i < PARAM_END; i++) {
        uint16_t cell = eeMap[i].cell;
        uint16_t last = eeFindLastCell(cell, EE_CELLS_NUM - 1);
        if (last != EE_NOT_FOUND) {
            uint16_t eeAddr = *ADDR(last);
            uint16_t eeData = *DATA(last);

            if (eeAddr == addr) {
                eeData = data;
            }

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

static void eeSwapPage(void)
{
    eeErasePages(currPage, EE_PAGE_STEP);

    currPage = copyPage;

    eeFormatPage(currPage);
}

void eeInit()
{
    LL_mDelay(1);

    uint16_t head0 = *(uint16_t *)(FLASH_BASE + EE_PAGE_SIZE * EE_PAGE_0);
    uint16_t head1 = *(uint16_t *)(FLASH_BASE + EE_PAGE_SIZE * EE_PAGE_1);

    currPage = EE_PAGE_0;

    if ((head0 != HEAD_VALID && head1 != HEAD_VALID) || (head0 == head1)) {
        eeErasePages(EE_PAGE_0, EE_PAGE_STEP * 2);
        eeFormatPage(EE_PAGE_0);
    } else if (HEAD_VALID == head0) {
        currPage = EE_PAGE_0;
    } else if (HEAD_VALID == head1) {
        currPage = EE_PAGE_1;
    }
}

void *eeGetPageAddr(uint16_t page)
{
    return (void *)PAGE_ADDR(page);
}

void eeErasePages(uint16_t page, uint16_t count)
{
    eeUnlock();
    eeWaitBusy();
    SET_BIT(FLASH->CR, FLASH_CR_PER);
    for (uint8_t i = 0; i < count; i++) {
        WRITE_REG(FLASH->AR, FLASH_BASE + EE_PAGE_SIZE * (page + i));
        SET_BIT(FLASH->CR, FLASH_CR_STRT);
        eeWaitBusy();
    }
    CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
    eeLock();
    eeWaitBusy();
}

void eeWritePage(uint16_t page, void *addr, uint16_t bytes)
{
    uint16_t *data = (uint16_t *)addr;
    uint16_t *cell = (uint16_t *)PAGE_ADDR(page);

    eeUnlock();
    eeWaitBusy();
    SET_BIT(FLASH->CR, FLASH_CR_PG);
    for (uint16_t i = 0; i < ((bytes + 1) / 2); i++) {
        cell[i] = data[i];
        eeWaitBusy();
    }
    CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
    eeLock();
}

uint16_t eeReadRaw(uint16_t cell)
{
    uint16_t ret = EE_NOT_FOUND;

    uint16_t emptyCell = eeFindEmptyCell();
    if (emptyCell == EE_NOT_FOUND) {
        emptyCell = EE_CELLS_NUM;
    }

    uint16_t last = eeFindLastCell(cell, emptyCell - 1);
    if (last == EE_NOT_FOUND) {
        ret = EE_NOT_FOUND;
    } else {
        ret = *DATA(last);
    }

    return ret;
}

void eeUpdateRaw(uint16_t cell, uint16_t data)
{
    uint16_t emptyCell = eeFindEmptyCell();

    if (emptyCell != EE_NOT_FOUND) {
        uint16_t lastCell = eeFindLastCell(cell, emptyCell - 1);
        if (lastCell == EE_NOT_FOUND || *DATA(lastCell) != data) {
            eeUnlock();
            eeWaitBusy();
            SET_BIT(FLASH->CR, FLASH_CR_PG);
            uint16_t *raw = ADDR(emptyCell);
            *raw = cell;
            eeWaitBusy();
            raw = DATA(emptyCell);
            *raw = data;
            eeWaitBusy();
            CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
            eeLock();
        }
    } else {
        uint16_t lastCell = eeFindLastCell(cell, EE_CELLS_NUM - 1);
        if (lastCell == EE_NOT_FOUND || *DATA(lastCell) != data) {
            eeCopyPage(cell, data);
            eeSwapPage();
        }
    }
}
