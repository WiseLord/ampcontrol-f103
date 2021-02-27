#include "i2cexp.h"

#include "hwlibs.h"
#include "i2c.h"
#include "menu.h"
#include "swtimers.h"

static I2cAddrIdx idxGpio = I2C_ADDR_DISABLED;

static Pcf8574Gpio pcf8574Gpio = 0x00;

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

void i2cExpGpioInit(void)
{
    idxGpio = (I2cAddrIdx)settingsGet(PARAM_I2C_EXT_GPIO);

    if (idxGpio != I2C_ADDR_DISABLED) {
        i2cExpSend(idxGpio, pcf8574Gpio);
    }
}

void i2cExpGpioKeyPress(Pcf8574Gpio gpio)
{
    pcf8574Gpio |= gpio;

    i2cExpSend(idxGpio, pcf8574Gpio);
    swTimSet(SW_TIM_GPIO_KEY, 200);
}

void i2cExpGpioKeyRelease(void)
{
    pcf8574Gpio &= ~BT_BTN_MASK;

    if (swTimGet(SW_TIM_GPIO_KEY) == 0) {
        i2cExpSend(idxGpio, pcf8574Gpio);
        swTimSet(SW_TIM_GPIO_KEY, SW_TIM_OFF);
    }
}
