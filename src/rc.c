#include "rc.h"

#include "hwlibs.h"

#include "eemul.h"
#include "settings.h"
#include "timers.h"

// Remote control pins
#define RC_Port                 GPIOA
#define RC_Pin                  LL_GPIO_PIN_8
#define RC_ExtiLine             LL_EXTI_LINE_8
#define RC_AR_ExtiPort          LL_GPIO_AF_EXTI_PORTA
#define RC_AR_ExtiLine          LL_GPIO_AF_EXTI_LINE8

static uint16_t rcCode[RC_CMD_END]; // Array with rc commands

typedef  uint8_t NecState;
enum {
    STATE_NEC_IDLE = 0,
    STATE_NEC_INIT,
    STATE_NEC_REPEAT,
    STATE_NEC_RECEIVE,
};

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

// RC5/RC6 definitions
#define RC6_1T                      444
#define RC6_2T                      889
#define RC6_3T                      1333
#define RC6_4T                      1778
#define RC6_6T                      2667

#define RC5_STBT_MASK               0x2000
#define RC5_FIBT_MASK               0x1000
#define RC5_TOGB_MASK               0x0800
#define RC5_ADDR_MASK               0x07C0
#define RC5_COMM_MASK               0x003F

#define RC6_ADDR_MASK               0xFFC0
#define RC6_COMM_MASK               0x00FF

#define RC6_DEV_MIN                 0.8
#define RC6_DEV_MAX                 1.2
#define RC6_MIN(delay)              ((uint16_t)((delay) * RC6_DEV_MIN))
#define RC6_MAX(delay)              ((uint16_t)((delay) * RC6_DEV_MAX))
#define RC6_NEAR(value, delay)      (value > RC6_MIN(delay) && value < RC6_MAX(delay))

typedef uint8_t RC5State;
enum {
    STATE_RC5_MID0 = 0,
    STATE_RC5_MID1,
    STATE_RC5_START0,
    STATE_RC5_START1,
};

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
                rcData.ready = true;
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
                if (0xFF == (seq.ncmd ^ seq.cmd)) {
                    ovfCnt = 0;
                    // Ready new data
                    rcData.type = seq.type;
                    rcData.addr = seq.addr;
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

static void rcDecodeRC56 (bool rc, uint16_t delay)
{
    // RC5/RC6 protocol variables
    static uint8_t rc5Cnt = 16;                     // RC5 bit counter
    static uint16_t rc5Cmd = 0;                     // RC5 command
    static RC5State rc5State = STATE_RC5_START1;    // RC5 decoding state
    static int8_t rc6Cnt = 0;                       // RC6 bit counter
    static uint16_t rc6Cmd = 0;                     // RC6 command
    static RC5State rc6State = STATE_RC5_START1;    // RC6 decoding state

    if (rc) {
        if (RC6_NEAR(delay, RC6_2T)) {
            if (rc5State == STATE_RC5_START1) {
                rc5State = STATE_RC5_MID1;
                rc5Cnt--;
                rc5Cmd <<= 1;
                rc5Cmd |= 0x01;
            } else if (rc5State == STATE_RC5_MID0) {
                rc5State = STATE_RC5_START0;
            }
            if (rc6State == STATE_RC5_MID1) {
                if (rc6Cnt == 21 || rc6Cnt == 16) {
                    rc6State = STATE_RC5_START1;
                } else {
                    rc6State = STATE_RC5_MID0;
                    if (--rc6Cnt < 16)
                        rc6Cmd <<= 1;
                }
            } else if (rc6State == STATE_RC5_START0) {
                if (rc6Cnt == 17) {
                    rc6State = STATE_RC5_MID0;
                    --rc6Cnt;
                }
            }
        } else if (RC6_NEAR(delay, RC6_4T)) {
            if (rc5State == STATE_RC5_MID0) {
                rc5State = STATE_RC5_MID1;
                rc5Cnt--;
                rc5Cmd <<= 1;
                rc5Cmd |= 0x01;
            }
        } else if (RC6_NEAR(delay, RC6_1T)) {
            rc5Cnt = 13;                            // Reset
            rc5State = STATE_RC5_MID1;
            if (rc6State == STATE_RC5_START0) {
                rc6State = STATE_RC5_MID0;
                if (--rc6Cnt < 16)
                    rc6Cmd <<= 1;
            } else if (rc6State == STATE_RC5_MID1) {
                rc6State = STATE_RC5_START1;
            }
        } else if (RC6_NEAR(delay, RC6_3T)) {
            rc5Cnt = 13;                            // Reset
            rc5State = STATE_RC5_MID1;
            if (rc6State == STATE_RC5_MID1) {
                if (rc6Cnt == 17) {
                    rc6State = STATE_RC5_MID0;
                    --rc6Cnt;
                } else if (rc6Cnt == 21 || rc6Cnt == 16) {
                    rc6State = STATE_RC5_MID0;
                    if (--rc6Cnt < 16) {
                        rc6Cmd <<= 1;
                    }
                }
            }
        } else {
            rc5Cnt = 13;                            // Reset
            rc5State = STATE_RC5_MID1;
            rc6Cnt = 22;                            // Reset
            rc6State = STATE_RC5_START1;
        }

    } else {
        // Try to decode as RC6 sequence
        if (RC6_NEAR(delay, RC6_2T)) {
            if (rc6State == STATE_RC5_MID0) {
                if (rc6Cnt == 16) {
                    rc6State = STATE_RC5_START0;
                } else {
                    rc6State = STATE_RC5_MID1;
                    if (--rc6Cnt < 16) {
                        rc6Cmd <<= 1;
                        rc6Cmd |= 0x01;
                    }
                }
            } else if (rc6State == STATE_RC5_START1) {
                if (rc6Cnt == 17) {
                    rc6State = STATE_RC5_MID1;
                    --rc6Cnt;
                }
            }
            if (rc5State == STATE_RC5_MID1) {
                rc5State = STATE_RC5_START1;
            } else if (rc5State == STATE_RC5_START0) {
                rc5State = STATE_RC5_MID0;
                rc5Cnt--;
                rc5Cmd <<= 1;
            }
        } else if (RC6_NEAR(delay, RC6_4T)) {
            if (rc5State == STATE_RC5_MID1) {
                rc5State = STATE_RC5_MID0;
                rc5Cnt--;
                rc5Cmd <<= 1;
            }
        } else if (RC6_NEAR(delay, RC6_1T)) {
            if (rc6State == STATE_RC5_START1) {
                rc6State = STATE_RC5_MID1;
                if (--rc6Cnt < 16) {
                    rc6Cmd <<= 1;
                    rc6Cmd |= 0x01;
                }
            } else if (rc6State == STATE_RC5_MID0) {
                rc6State = STATE_RC5_START0;
            }
        } else if (RC6_NEAR(delay, RC6_3T)) {
            if (rc6State == STATE_RC5_MID0) {
                if (rc6Cnt == 17) {
                    rc6State = STATE_RC5_MID1;
                    --rc6Cnt;
                } else if (rc6Cnt == 16) {
                    rc6State = STATE_RC5_MID1;
                    rc6Cmd <<= 1;
                    rc6Cmd |= 0x01;
                }
            }
        } else if (RC6_NEAR(delay, RC6_6T)) {
            rc6State = STATE_RC5_MID1;
            rc6Cnt = 21;
        } else {
            rc6Cnt = 22;                            // Reset
            rc6State = STATE_RC5_START1;
        }

    }

    if (rc5Cnt == 0 || rc6Cnt == 0) {
        if (rc5Cnt == 0) {
            rcData.type = RC_TYPE_RC5;
            rcData.addr  = (rc5Cmd & RC5_ADDR_MASK) >> 6;
            rcData.cmd = (rc5Cmd & RC5_COMM_MASK) | (rc5Cmd & RC5_FIBT_MASK ? 0x00 : 0x40);
        } else {
            rcData.type = RC_TYPE_RC6;
            rcData.addr  = (rc6Cmd & RC6_ADDR_MASK) >> 8;
            rcData.cmd = rc6Cmd & RC6_COMM_MASK;
        }
        rc6Cnt = 22;
        rc5Cnt = 13;
        rcData.ready = true;
    }
}

static void rcInitPins(void)
{
#ifdef STM32F1
    LL_GPIO_AF_SetEXTISource(RC_AR_ExtiPort, RC_AR_ExtiLine);
    LL_GPIO_SetPinMode(RC_Port, RC_Pin, LL_GPIO_MODE_FLOATING);
#endif

    LL_EXTI_DisableEvent_0_31(RC_ExtiLine);
    LL_EXTI_EnableIT_0_31(RC_ExtiLine);
    LL_EXTI_EnableRisingTrig_0_31(RC_ExtiLine);
    LL_EXTI_EnableFallingTrig_0_31(RC_ExtiLine);
}

void rcReadSettings(void)
{
    for (RcCmd cmd = 0; cmd < RC_CMD_END; cmd++) {
        rcCode[cmd] = settingsRead(PARAM_RC_STBY_SWITCH + cmd, EE_NOT_FOUND);
    }
}

void rcInit(void)
{
    rcReadSettings();
    rcInitPins();

    timerInit(TIM_RC, 71, 65535); // 1MHz timer for remote control handling

    NVIC_SetPriority(EXTI9_5_IRQn, 0);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

static void rcIRQ(void)
{
    static uint32_t timCntOld = 0;
    uint32_t timCnt = LL_TIM_GetCounter(TIM_RC);
    uint16_t delay = (uint16_t)(timCnt - timCntOld);
    timCntOld = timCnt;

    bool rc = !(LL_GPIO_ReadInputPort(RC_Port) & LL_EXTI_LINE_8);

    rcDecodeNecSam(rc, delay);
    rcDecodeRC56(rc, delay);
}

void EXTI_RC_HANDLER()
{
    if (LL_EXTI_IsActiveFlag_0_31(RC_ExtiLine) != RESET) {
        // Clear RC line interrupt
        LL_EXTI_ClearFlag_0_31(RC_ExtiLine);

        // Callback
        rcIRQ();
    }
}

void TIM_RC_HANDLER(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM_RC)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM_RC);

        if (ovfCnt <= 1000) {
            ovfCnt++;
        }
    }
}

RcData rcRead(bool clear)
{
    RcData ret = rcData;

    if (clear) {
        rcData.ready = 0;
    }

    return ret;
}

uint16_t rcGetCode(RcCmd cmd)
{
    if (cmd >= RC_CMD_END)
        return EE_NOT_FOUND;

    return rcCode[cmd];
}

void rcSaveCode(uint16_t cmd, uint16_t value)
{
    if (cmd >= RC_CMD_END)
        return;

    rcCode[cmd] = value;
    settingsStore(PARAM_RC_STBY_SWITCH + cmd, value);
}

RcCmd rcGetCmd(RcData *rcData)
{
    uint16_t raw = ((rcData->addr << 8) & 0xFF00) | (rcData->cmd & 0x00FF);

    RcCmd cmd;
    for (cmd = 0; cmd < RC_CMD_END; cmd++) {
        if (rcCode[cmd] == raw) {
            return cmd;
        }
    }

    return RC_CMD_END;
}
