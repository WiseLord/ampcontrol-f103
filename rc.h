#ifndef RC_H
#define RC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "cmd.h"

#define GENERATE_RC_CMD(RC)    RC_CMD_ ## RC,

typedef enum {
    RC_TYPE_NONE = 0,

    RC_TYPE_NEC,
    RC_TYPE_SAM,
    RC_TYPE_RC5,
    RC_TYPE_RC6,

    RC_TYPE_END
} RcType;

typedef struct {
    RcType type;
    uint16_t addr;
    uint8_t cmd;
    bool repeat;
    bool ready;
} RcData;

typedef uint16_t RcCmd;
enum {
    FOREACH_CMD(GENERATE_RC_CMD)

    RC_CMD_END
};

void rcInit(void);
void rcIRQ(void);
void rcOvfIRQ(void);

RcData rcRead(bool clear);

uint16_t rcGetCode(RcCmd cmd);
void rcSaveCode(uint16_t cmd, uint16_t value);

RcCmd rcGetCmd(RcData *rcData);

#ifdef __cplusplus
}
#endif

#endif // RC_H
