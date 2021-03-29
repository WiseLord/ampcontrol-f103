#ifndef I2CEXP_H
#define I2CEXP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define PCF8574_RELEASED    0x00

typedef uint8_t I2cExpKey;

// Bluetooth module buttons
#define BT_NEXT_TRACK   (1 << 5)
#define BT_PREV_TRACK   (1 << 4)
#define BT_NEXT_INPUT   (1 << 3)
#define BT_PLAY_PAUSE   (1 << 2)

#define BT_BTN_MASK         (BT_NEXT_TRACK | BT_PREV_TRACK | BT_NEXT_INPUT | BT_PLAY_PAUSE);

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

typedef struct {
    I2cAddrIdx idxInStatus;
    I2cAddrIdx idxGpio;
    I2cExpKey keys;
} I2CExp;

uint8_t i2cExpGetAddr(I2cAddrIdx idx);
void i2cExpSend(I2cAddrIdx idx, uint8_t data);

void i2cExpInit(void);
I2CExp *i2cExpGet(void);

void i2cExpGpioKeyPress(I2cExpKey key);
void i2cExpGpioKeyRelease(void);

#ifdef __cplusplus
}
#endif

#endif // I2CEXP_H
