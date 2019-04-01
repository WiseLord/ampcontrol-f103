#ifndef HX8347D_H
#define HX8347D_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void hx8347dInit(void);

void hx8347dSleep(void);
void hx8347dWakeup(void);

void hx8347dSetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // HX8347D_H
