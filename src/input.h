#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define BTN_NO                  0x0000
#define ENC_NO                  BTN_NO

#define BTN_D0                  0x0001
#define BTN_D1                  0x0002
#define BTN_D2                  0x0004
#define BTN_D3                  0x0008
#define BTN_D4                  0x0010
#define BTN_D5                  0x0020
#define BTN_ALL                 (BTN_D0 | BTN_D1 | BTN_D2 | BTN_D3 | BTN_D4 | BTN_D5)

#define ENC_A                   0x0040
#define ENC_B                   0x0080
#define ENC_AB                  (ENC_A | ENC_B)

// Handling long press actions
#define SHORT_PRESS             60
#define LONG_PRESS              600
#define AUTOREPEAT              150

#define ENC_RES_MIN             -64
#define ENC_RES_MAX             64

#define BTN_FLAG_NO             0x0000
#define BTN_FLAG_LONG_PRESS     0x0001

// Input pins are shared with display data port
#define INPUT_Port              GPIOB
#define INPUT_Pin               (LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
                                 LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7)

typedef struct {
    uint16_t btn;
    uint16_t flags;
} CmdBtn;

typedef struct {
    uint16_t btn;
    uint16_t flags;
    int8_t encCnt;
    int8_t encRes;
} Input;

void inputInit(void);
Input *inputGet(void);

int8_t inputGetEncoder(void);
CmdBtn inputGetBtnCmd(void);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
