#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define LINE_SIZE       128

typedef struct {
    char line[LINE_SIZE];
    int16_t idx;
    int16_t size;
} LineParse;

void utilmDelay(uint32_t delay);

char *utilMkStr(const char *fmt, ...);

bool utilReadChar(LineParse *lp, char ch);

#ifdef __cplusplus
}
#endif

#endif // UTILS_H
