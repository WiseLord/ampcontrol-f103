#ifndef L2F50126_H
#define L2F50126_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void l2f50126Init(DispDriver **driver);

void l2f50126Sleep(void);
void l2f50126Wakeup(void);

void l2f50126SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // L2F50126_H
