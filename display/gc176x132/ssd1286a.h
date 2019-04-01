#ifndef SSD1286A_H
#define SSD1286A_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ssd1286aInit(void);

void ssd1286aSleep(void);
void ssd1286aWakeup(void);

void ssd1286aSetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // SSD1286A_H
