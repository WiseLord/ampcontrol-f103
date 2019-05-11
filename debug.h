#ifndef DEBUG_H
#define DEBUG_H

#ifdef _DEBUG_ENABLED
#define DBG(fmt, ...)  do { dbgPrintf(fmt, ## __VA_ARGS__); } while(0)
#else
#define DBG(fmt, ...)
#endif

void dbgInit(void);
void dbgPrintf (const char *fmt, ...);

#endif // DEBUG_H
