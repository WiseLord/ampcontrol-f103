#ifndef RC_H
#define RC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void rcInit(void);
void rcIRQ(void);

bool rcGetValue(void);

#ifdef __cplusplus
}
#endif

#endif // RC_H
