#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define LINE_SIZE       128

typedef struct {
    char line[LINE_SIZE];
    int16_t idx;
    int16_t size;
} LineParse;

void utilInitSysCounter(void);

void utilmDelay(uint32_t ms);
void utiluDelay(uint32_t us);

bool utilReadChar(LineParse *lp, char ch);

bool utilIsPrefix(const char *line, const char *prefix);
void utilTrimLineEnd(char *line);

#ifdef __cplusplus
}
#endif

#endif // UTILS_H
