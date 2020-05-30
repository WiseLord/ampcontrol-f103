#ifndef RC_H
#define RC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "action.h"

#define GENERATE_RC_CMD(RC)     RC_CMD_ ## RC,

// Remote control pins
#define RC_Port                 GPIOA
#define RC_Pin                  LL_GPIO_PIN_8
#define RC_ExtiLine             LL_EXTI_LINE_8
#define RC_AR_ExtiPort          LL_GPIO_AF_EXTI_PORTA
#define RC_AR_ExtiLine          LL_GPIO_AF_EXTI_LINE8

typedef uint8_t RcType;
enum {
    RC_TYPE_NONE = 0,

    RC_TYPE_NEC,
    RC_TYPE_SAM,
    RC_TYPE_RC5,
    RC_TYPE_RC6,

    RC_TYPE_END
};

typedef struct {
    RcType type;
    uint16_t addr;
    uint8_t cmd;
    bool ready;
} RcData;

typedef uint16_t RcCmd;
enum {
    FOREACH_CMD(GENERATE_RC_CMD)

    RC_CMD_END
};

void rcInit(void);

RcData rcRead(bool clear);

uint16_t rcGetCode(RcCmd cmd);
void rcSaveCode(uint16_t cmd, uint16_t value);

RcCmd rcGetCmd(RcData *rcData);

#ifdef __cplusplus
}
#endif

#endif // RC_H
