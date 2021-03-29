#include "i2cexp.h"

#include "hwlibs.h"
#include "i2c.h"
#include "menu.h"
#include "swtimers.h"

static I2CExp i2cExp;

uint8_t i2cExpGetAddr(I2cAddrIdx idx)
{
    uint8_t addr = 0x40;

    if (idx <= PCF8574_0x4E) {
        addr = 0x40 | (uint8_t)((idx - PCF8574_0x40) << 1);
    } else if (idx <= PCF8574A_0x7E) {
        addr = 0x70 | (uint8_t)((idx - PCF8574A_0x70) << 1);
    }

    return addr;
}

void i2cExpSend(I2cAddrIdx idx, uint8_t data)
{
    if (idx == I2C_ADDR_DISABLED || idx >= I2C_ADDR_END) {
        return;
    }

    uint8_t i2cAddr = i2cExpGetAddr(idx);

    i2cBegin(I2C_AMP, i2cAddr);
    i2cSend(I2C_AMP, data);
    i2cTransmit(I2C_AMP);
}

void i2cExpInit(void)
{
    i2cExp.idxInStatus = settingsRead(PARAM_I2C_EXT_IN_STAT, I2C_ADDR_DISABLED);
    i2cExp.idxGpio = settingsRead(PARAM_I2C_EXT_GPIO, I2C_ADDR_DISABLED);

    if (i2cExp.idxGpio != I2C_ADDR_DISABLED) {
        i2cExpSend(i2cExp.idxGpio, i2cExp.keys);
    }
}

I2CExp *i2cExpGet(void)
{
    return &i2cExp;
}

void i2cExpGpioKeyPress(I2cExpKey key)
{
    i2cExp.keys |= key;

    i2cExpSend(i2cExp.idxGpio, i2cExp.keys);
    swTimSet(SW_TIM_GPIO_KEY, 200);
}

void i2cExpGpioKeyRelease(void)
{
    i2cExp.keys &= ~BT_BTN_MASK;

    if (swTimGet(SW_TIM_GPIO_KEY) == 0) {
        i2cExpSend(i2cExp.idxGpio, i2cExp.keys);
        swTimSet(SW_TIM_GPIO_KEY, SW_TIM_OFF);
    }
}
