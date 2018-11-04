#ifndef RC_H
#define RC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    RC_TYPE_NONE = 0,

    RC_TYPE_NEC,
    RC_TYPE_SAM,

    RC_TYPE_END
} RcType;

typedef struct {
    RcType type;
    uint16_t addr;
    uint8_t cmd;
    bool repeat;
    bool ready;
} RcData;

void rcInit(void);
void rcIRQ(void);
void rcOvfIRQ(void);

RcData rcRead(bool clear);

#ifdef __cplusplus
}
#endif

#endif // RC_H
