#ifndef AMP_H
#define AMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint8_t AmpStatus;
enum {
    AMP_STATUS_STBY,
    AMP_STATUS_POWERED,
    AMP_STATUS_INIT,
    AMP_STATUS_HW_READY,
    AMP_STATUS_ACTIVE,

    AMP_STATUS_END
};

AmpStatus ampGetStatus(void);

void ampInit(void);
void ampExitStby(void);
void ampEnterStby(void);
void ampInitHw(void);

void ampSetInput(uint8_t value);

#ifdef __cplusplus
}
#endif

#endif // AMP_H
