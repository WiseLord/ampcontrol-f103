#ifndef SCROLLTEXT_H
#define SCROLLTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "display/glcd.h"

typedef uint8_t ScrollTextFlags;
enum {
    SCROLL_NO_FLAG  = 0x00,

    SCROLL_EVENT    = 0x01,
    SCROLL_RIGHT    = 0x02,
};

typedef struct {
    const GlcdRect *rect;
    const char *text;
    int16_t oft;
    uint8_t pause;
    ScrollTextFlags flags;
} ScrollText;

void scrollTextDraw(ScrollText *this, bool clear);

#ifdef __cplusplus
}
#endif

#endif // SCROLLTEXT_H
