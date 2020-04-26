#ifndef STRIPEDBAR_H
#define STRIPEDBAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef const struct {
    int16_t sc;                 // Count of bar lines
    uint8_t sw;                 // Width of bar line
    int16_t barY;               // Y pos of the bar
    int16_t barW;               // Width of the bar
    uint8_t half;               // Height of upper/lower bar part
    uint8_t middle;             // Height of middle bar part
} LayoutStripedBar;

typedef struct {
    int16_t value;
    int16_t min;
    int16_t max;
} StripedBar;

void stripedBarDraw(bool clear, StripedBar *bar, LayoutStripedBar *lt);

#ifdef __cplusplus
}
#endif

#endif // STRIPEDBAR_H
