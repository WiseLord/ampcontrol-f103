#include "rc.h"
#include "pins.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_tim.h>

typedef enum {
    STATE_NEC_IDLE = 0,
    STATE_NEC_INIT,
    STATE_NEC_REPEAT,
    STATE_NEC_RECEIVE,
} NecState;

typedef struct {
    union {
        uint32_t raw;
        uint16_t addr;
        struct {
            uint8_t laddr;
            uint8_t haddr;
            uint8_t cmd;
            uint8_t ncmd;
        };
    };
    RcType type;        // Currently decoding type (NEC, Samsung)
    NecState state;     // Current decoding state
    uint8_t bit;
} NecSequence;

// NEC/Samsung definitions
#define NEC_INIT                    9000
#define SAM_INIT                    4500
#define NEC_START                   4500
#define NEC_REPEAT                  2250
#define NEC_ZERO                    560
#define NEC_ONE                     1680
#define NEC_PULSE                   560

#define NEC_DEV_MIN                 0.75
#define NEC_DEV_MAX                 1.25
#define NEC_MIN(delay)              ((uint16_t)((delay) * NEC_DEV_MIN))
#define NEC_MAX(delay)              ((uint16_t)((delay) * NEC_DEV_MAX))
#define NEC_NEAR(value, delay)      ((value) > NEC_MIN(delay) && (value) < NEC_MAX(delay))

#define NEC_REPEAT_LIMIT            10

static uint16_t ovfCnt;

static RcData rcData;

static void rcDecodeNecSam (bool rc, uint16_t delay)
{
    static NecSequence seq;

    if (rc) {
        if (seq.state == STATE_NEC_INIT) {
            seq.bit = 0;

            if (NEC_NEAR(delay, NEC_START)) {
                seq.state = STATE_NEC_RECEIVE;
            } else if (NEC_NEAR(delay, NEC_REPEAT) && ovfCnt < NEC_REPEAT_LIMIT) {
                ovfCnt = 0;
                // Ready repeated data
                rcData.repeat = 1;
                rcData.ready = 1;
            }
        } else if (seq.state == STATE_NEC_RECEIVE) {
            seq.bit++;

            seq.raw >>= 1;
            if (NEC_NEAR(delay, NEC_ZERO)) {
                seq.raw &= ~0x80000000;
            } else if (NEC_NEAR(delay, NEC_ONE)) {
                seq.raw |= 0x80000000;
            } else {
                seq.bit = 0;
            }

            if (seq.bit == 32) {
                if ((uint8_t)(~seq.ncmd) == seq.cmd) {
                    ovfCnt = 0;
                    // Ready new data
                    rcData.type = seq.type;
                    if (ovfCnt < NEC_REPEAT_LIMIT)
                        rcData.repeat = 1;
                    else
                        rcData.repeat = 0;
                    if ((uint8_t)(~seq.haddr) == seq.laddr) {
                        rcData.addr = seq.laddr;
                    } else {
                        rcData.addr = seq.addr;
                    }
                    rcData.cmd = seq.cmd;

                    rcData.ready = 1;
                }
            }
        }
    } else {
        if (NEC_NEAR(delay, NEC_PULSE) && seq.state != STATE_NEC_REPEAT) {
            seq.state = STATE_NEC_RECEIVE;
        } else if (NEC_NEAR(delay, NEC_INIT)) {
            seq.state = STATE_NEC_INIT;
            seq.type = RC_TYPE_NEC;
        } else if (NEC_NEAR(delay, SAM_INIT)) {
            seq.state = STATE_NEC_INIT;
            seq.type = RC_TYPE_SAM;
        } else {
            seq.state = STATE_NEC_IDLE;
        }
    }
}


void rcInit(void)
{
    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void rcIRQ()
{
    static uint16_t timCntOld = 0;
    uint16_t timCnt = LL_TIM_GetCounter(TIM3);
    uint16_t delay = timCnt - timCntOld;
    timCntOld = timCnt;

    bool rc = !READ(RC);

    rcDecodeNecSam(rc, delay);
}

void rcOvfIRQ(void)
{
    if (ovfCnt <= 1000)
        ovfCnt++;
}


RcData rcRead(bool clear)
{
    RcData ret = rcData;

    if (clear) {
        rcData.ready = 0;
    }

    return ret;
}
