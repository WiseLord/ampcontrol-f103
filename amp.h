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

typedef uint8_t I2cAddrIdx;
enum {
    I2C_ADDR_DISABLED,

    PCF8574_0x40,
    PCF8574_0x42,
    PCF8574_0x44,
    PCF8574_0x46,
    PCF8574_0x48,
    PCF8574_0x4A,
    PCF8574_0x4C,
    PCF8574_0x4E,

    PCF8574A_0x70,
    PCF8574A_0x72,
    PCF8574A_0x74,
    PCF8574A_0x76,
    PCF8574A_0x78,
    PCF8574A_0x7A,
    PCF8574A_0x7C,
    PCF8574A_0x7E,

    I2C_ADDR_END,
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
