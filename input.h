#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define BTN_NO                  0x00

#define BTN_D0                  0x01
#define BTN_D1                  0x02
#define BTN_D2                  0x04
#define BTN_D3                  0x08
#define BTN_D4                  0x10
#define BTN_D5                  0x20
#define BTN_D6                  0x40
#define BTN_D7                  0x80

#define ENC_NO                  BTN_NO
#define ENC_A                   BTN_D6
#define ENC_B                   BTN_D7
#define ENC_AB                  (ENC_A | ENC_B)

// Handling long press actions
#define SHORT_PRESS             100
#define LONG_PRESS              600
#define AUTOREPEAT              150

#define ENC_RES_MIN             -64
#define ENC_RES_MAX             64
#define ENC_RES_DEFAULT         4

typedef uint16_t CmdBtn;

void inputInit(void);
void inputSetEncRes(int8_t value);
void inputPoll(void);

int8_t getEncoder(void);
CmdBtn getBtnCmd(void);

#endif // INPUT_H
