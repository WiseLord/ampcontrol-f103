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
    int32_t idx;
} LineParse;

void utilmDelay(uint32_t delay);

char *utilMkStr(const char *fmt, ...);

bool utilReadChar(LineParse *lp, char data);

#ifdef __cplusplus
}
#endif

#endif // UTILS_H
