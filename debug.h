#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#define USART_DBG               USART1

#include <stdint.h>

void dbgInit(void);
char dbgGetChar(void);
void dbg(const char *str);

#ifdef __cplusplus
}
#endif

#endif // DEBUG_H
