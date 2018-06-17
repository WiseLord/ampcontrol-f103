#include "input.h"

#include "gdfb.h"
#include "pins.h"

// Previous state
static volatile uint8_t encPrev = ENC_NO;
static volatile uint8_t btnPrev = BTN_NO;

static volatile int8_t encRes = 0;
static volatile int8_t encCnt = 0;

static CmdBtn cmdBuf = CMD_BTN_END;

static uint8_t getPins()
{
    return gdGetPins();
}

void inputInit()
{
    encRes = -4; // TODO: Read from settings
    encCnt = 0;
    cmdBuf = CMD_BTN_END;
}

void inputPoll()
{
    static int16_t btnCnt = 0;                      // Buttons press duration value

    uint8_t btnNow = getPins();
    uint8_t encNow = btnNow;

    // If encoder event has happened, inc/dec encoder counter
    if (encRes) {
        encNow = btnNow & ENC_AB;
        btnNow &= ~ENC_AB;

        if ((encPrev == ENC_NO && encNow == ENC_A) ||
                (encPrev == ENC_A && encNow == ENC_AB) ||
                (encPrev == ENC_AB && encNow == ENC_B) ||
                (encPrev == ENC_B && encNow == ENC_NO))
            encCnt++;
        if ((encPrev == ENC_NO && encNow == ENC_B) ||
                (encPrev == ENC_B && encNow == ENC_AB) ||
                (encPrev == ENC_AB && encNow == ENC_A) ||
                (encPrev == ENC_A && encNow == ENC_NO))
            encCnt--;
        encPrev = encNow;
    }

    // If button event has happened, place it to command buffer
    if (btnNow) {
        if (btnNow == btnPrev) {
            btnCnt++;
            if (btnCnt == LONG_PRESS) {
                switch (btnPrev) {
                case BTN_D0:
                    cmdBuf = CMD_BTN_0_LONG;
                    break;
                case BTN_D1:
                    cmdBuf = CMD_BTN_1_LONG;
                    break;
                case BTN_D2:
                    cmdBuf = CMD_BTN_2_LONG;
                    break;
                case BTN_D3:
                    cmdBuf = CMD_BTN_3_LONG;
                    break;
                case BTN_D4:
                    cmdBuf = CMD_BTN_4_LONG;
                    break;
                case BTN_D5:
                    cmdBuf = CMD_BTN_5_LONG;
                    break;
                }
            } else if (!encRes) {
                if (btnCnt == LONG_PRESS + AUTOREPEAT) {
                    switch (btnPrev) {
                    case ENC_A:
                        encCnt++;
                        break;
                    case ENC_B:
                        encCnt--;
                        break;
                    }
                    btnCnt = LONG_PRESS + 1;
                }
            }
        } else {
            btnPrev = btnNow;
        }
    } else {
        if ((btnCnt > SHORT_PRESS) && (btnCnt < LONG_PRESS)) {
            switch (btnPrev) {
            case BTN_D0:
                cmdBuf = CMD_BTN_0;
                break;
            case BTN_D1:
                cmdBuf = CMD_BTN_1;
                break;
            case BTN_D2:
                cmdBuf = CMD_BTN_2;
                break;
            case BTN_D3:
                cmdBuf = CMD_BTN_3;
                break;
            case BTN_D4:
                cmdBuf = CMD_BTN_4;
                break;
            case BTN_D5:
                cmdBuf = CMD_BTN_5;
                break;
            }
            if (!encRes) {
                switch (btnPrev) {
                case ENC_A:
                    encCnt++;
                    break;
                case ENC_B:
                    encCnt--;
                    break;
                }
            }
        }
        btnCnt = 0;
    }
    btnPrev = btnNow;
}

int8_t getEncoder()
{
    int8_t ret = 0;

    if (encRes) {
        if (encRes > 0) {
            while (encCnt >= encRes) {
                ret++;
                encCnt -= encRes;
            }
            while (encCnt <= -encRes) {
                ret--;
                encCnt += encRes;
            }
        } else {
            while (encCnt <= encRes) {
                ret++;
                encCnt -= encRes;
            }
            while (encCnt >= -encRes) {
                ret--;
                encCnt += encRes;
            }
        }
    } else {
        ret = encCnt;
        encCnt = 0;
    }

    return ret;
}

CmdBtn getBtnCmd()
{
    CmdBtn ret = cmdBuf;
    cmdBuf = CMD_BTN_END;

    return ret;
}
