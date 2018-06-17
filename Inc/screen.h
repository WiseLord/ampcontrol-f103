#ifndef SCREEN_H
#define SCREEN_H

#include <inttypes.h>

#include "gdfb.h"

#define writeString(x)          gdWriteString(x)
// String buffer
#define STR_BUFSIZE             20

void screenTime();

#endif // SCREEN_H
