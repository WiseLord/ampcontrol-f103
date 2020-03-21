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

// TRUE if end of line
bool utilReadChar(LineParse *lp, char ch)
{
    if (lp->idx == 0) {
        lp->size = 0;
    }

    if (lp->idx + 2 >= LINE_SIZE) {
        lp->line[lp->idx + 1] = ch;
        lp->idx = 0;
        return true;
    } else if (ch == '\n') {
        lp->line[lp->idx] = '\0';
        lp->idx = 0;
        return true;
    } else {
        lp->line[lp->idx++] = ch;
        lp->size++;
        return false;
    }
}

bool utilIsPrefix(const char *line, const char *prefix)
{
    char p;
    char l = '\0';

    bool ret = true;

    while (line && (p = *prefix++) && (l = *line++)) {
        if (p != l) {
            ret = false;
            break;
        }
    }

    if (!l) {
        ret = false;
    }

    return ret;
}

void utilTrimLineEnd(char *line)
{
    size_t len = strlen(line);

    // Remove possible service symbols and spaces at the end
    while (len > 0 && (line[len - 1] <= ' ')) {
        line[len - 1] = '\0';
        len--;
    }
}
