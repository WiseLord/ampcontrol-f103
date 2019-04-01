#ifndef MC2PA8201_H
#define MC2PA8201_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void mc2pa8201Init(void);

void mc2pa8201Sleep(void);
void mc2pa8201Wakeup(void);

void mc2pa8201SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // MC2PA8201_H
