#ifndef EEEMUL_H
#define EEEMUL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define EE_PAGE_STEP    2
#define EE_PAGE_SIZE    1024

enum {
    EE_PAGE_0 = 124,
    EE_PAGE_1 = 126,
    EE_PAGE_END
};

void eeInit(void);

void eeUpdateRaw(uint16_t addr, uint16_t data);
uint16_t eeReadRaw(uint16_t addr);

#endif // EEEMUL_H
