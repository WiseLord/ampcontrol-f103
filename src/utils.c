#include "utils.h"

#include "hwlibs.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static char strbuf[256];    // String buffer

void utilmDelay(uint32_t delay)
{
    LL_mDelay(delay);
}

char *utilMkStr(const char *fmt, ...)
{
    char *buffer = strbuf;

    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end (args);

    return buffer;
}

bool utilReadChar(LineParse *lp, char ch)
{
    if (ch < ' ') {
        lp->line[lp->idx] = '\0';
        lp->idx = 0;
        return true;
    } else {
        lp->line[lp->idx++] = ch;
        if (lp->idx >= LINE_SIZE) {
            lp->line[--lp->idx] = '\0';
            lp->idx = 0;
        }
        return false;
    }
}
