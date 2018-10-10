#include "input.h"

#include "display/dispdrv.h"
#include "pins.h"

static volatile int8_t encRes = 0;
static volatile int8_t encCnt = 0;

static CmdBtn cmdBuf = BTN_NO;

void inputInit(void)
{
    encRes = -4; // TODO: Read from settings
    encCnt = 0;
    cmdBuf = BTN_NO;
}

void inputPoll(void)
{
    // Antibounce counter
    static int16_t btnCnt = 0;

    // Previous state
    static volatile uint8_t btnPrev = BTN_NO;
    static volatile uint8_t encPrev = ENC_NO;

    // Current state
    uint8_t btnNow = dispdrvGetBus();

    // If encoder event has happened, inc/dec encoder counter
    if (encRes) {
        uint8_t encNow = btnNow & ENC_AB;
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

    // On button event place it to command buffer
    if (btnNow) {
        if (btnNow == btnPrev) {
            btnCnt++;
            if (btnCnt == LONG_PRESS) {
                cmdBuf = btnPrev << 8;
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
            cmdBuf = btnPrev;
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

int8_t getEncoder(void)
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

CmdBtn getBtnCmd(void)
{
    CmdBtn ret = cmdBuf;
    cmdBuf = BTN_NO;

    return ret;
}
