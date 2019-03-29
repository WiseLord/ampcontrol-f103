#ifndef HX8347A_H
#define HX8347A_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void hx8347aInit(DispDriver **driver);

void hx8347aSleep(void);
void hx8347aWakeup(void);

void hx8347aSetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // HX8347A_H
