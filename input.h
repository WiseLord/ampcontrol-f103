#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define BTN_NO                  0x00
#define ENC_NO                  BTN_NO

#define BTN_D0                  0x01
#define BTN_D1                  0x02
#define BTN_D2                  0x04
#define BTN_D3                  0x08
#define BTN_D4                  0x10
#define BTN_D5                  0x20

#define ENC_A                   0x40
#define ENC_B                   0x80
#define ENC_AB                  (ENC_A | ENC_B)

// Handling long press actions
#define SHORT_PRESS             60
#define LONG_PRESS              600
#define AUTOREPEAT              150

#define ENC_RES_MIN             -64
#define ENC_RES_MAX             64

typedef uint16_t CmdBtn;

void inputInit(void);

void inputSetEncRes(int8_t value);
int8_t inputGetEncRes(void);

void inputPoll(void);

int8_t getEncoder(void);
CmdBtn getBtnCmd(void);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
