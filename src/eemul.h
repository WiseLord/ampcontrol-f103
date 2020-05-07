#ifndef EEEMUL_H
#define EEEMUL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define EE_EMPTY        (uint16_t)0xFFFF
#define EE_NOT_FOUND    (uint16_t)0x8000

#ifdef _F103C8
#define EE_PAGE_SIZE    1024
#define EE_PAGE_STEP    2
#define EE_PAGE_COUNT   64
#endif

#ifdef _F103CB
#define EE_PAGE_SIZE    1024
#define EE_PAGE_STEP    2
#define EE_PAGE_COUNT   128
#endif

#ifdef _F303CB
#define EE_PAGE_SIZE    2048
#define EE_PAGE_STEP    1
#define EE_PAGE_COUNT   64
#endif

#ifdef _F303CC
#define EE_PAGE_SIZE    2048
#define EE_PAGE_STEP    2
#define EE_PAGE_COUNT   128
#endif

typedef struct {
    uint16_t cell;
    int16_t value;
} EE_Cell;

void eeInit(const EE_Cell *map, uint16_t mapSize);

void *eeGetPageAddr(uint16_t page);
void eeErasePages(uint16_t page, uint16_t count);
void eeWritePage(uint16_t page, void *addr, uint16_t bytes);

void eeUpdateRaw(uint16_t cell, uint16_t data);
uint16_t eeReadRaw(uint16_t cell);

#ifdef __cplusplus
}
#endif

#endif // EEEMUL_H
