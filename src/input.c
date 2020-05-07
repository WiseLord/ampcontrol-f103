#include "input.h"

#include "display/dispdrv.h"
#include "settings.h"
#include "timers.h"

static Input input;

static void inputHandleButtons(uint16_t btnNow)
{
    // Antibounce counter
    static int16_t btnCnt = 0;

    // Previous state
    static uint16_t btnPrev = BTN_NO;

    // On button event place it to command buffer
    if (btnNow) {
        if (btnNow == btnPrev) {
            btnCnt++;
            if (btnCnt == LONG_PRESS) {
                input.btn = btnPrev;
                input.flags |= BTN_FLAG_LONG_PRESS;
                if (btnNow & (BTN_D3 | BTN_D4 | ENC_A | ENC_B)) {
                    btnCnt -= AUTOREPEAT;
                }
            }
        }
    } else {
        if ((btnCnt > SHORT_PRESS) && (btnCnt < LONG_PRESS - AUTOREPEAT)) {
            input.btn = btnPrev;
        }
        btnCnt = 0;
    }
    btnPrev = btnNow;
}

static void inputHandleEncoder(uint16_t encNow)
{
    // Previous state
    static uint16_t encPrev = ENC_NO;

    // If encoder event has happened, inc/dec encoder counter
    if (input.encRes) {
        if ((encPrev == ENC_NO && encNow == ENC_A) ||
            (encPrev == ENC_A && encNow == ENC_AB) ||
            (encPrev == ENC_AB && encNow == ENC_B) ||
            (encPrev == ENC_B && encNow == ENC_NO))
            input.encCnt++;
        if ((encPrev == ENC_NO && encNow == ENC_B) ||
            (encPrev == ENC_B && encNow == ENC_AB) ||
            (encPrev == ENC_AB && encNow == ENC_A) ||
            (encPrev == ENC_A && encNow == ENC_NO))
            input.encCnt--;
        encPrev = encNow;
    }
}

void inputInit(void)
{
    timerInit(TIM_INPUT, 199, 359);  // 1kHz polling

    input.encRes = (int8_t)settingsGet(PARAM_SYSTEM_ENC_RES);
}

Input *inputGet()
{
    return &input;
}

void TIM_INPUT_HANDLER(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM_INPUT)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM_INPUT);

        uint8_t dispBus = ~dispdrvGetBus();

        inputHandleButtons(dispBus & BTN_ALL);
        inputHandleEncoder(dispBus & ENC_AB);
    }
}

int8_t inputGetEncoder(void)
{
    int8_t ret = 0;

    if (input.encRes) {
        ret = input.encCnt / input.encRes;
        input.encCnt -= (ret * input.encRes);
    } else {
        ret = input.encCnt;
        input.encCnt = 0;
    }

    return ret;
}

CmdBtn inputGetBtnCmd(void)
{
    CmdBtn ret =  { .btn = input.btn, .flags = input.flags };

    input.btn = BTN_NO;
    input.flags = BTN_FLAG_NO;

    return ret;
}
