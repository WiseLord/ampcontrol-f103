#include "i2cexp.h"

#include "hwlibs.h"
#include "i2c.h"

uint8_t i2cexpGetAddr(I2cAddrIdx idx)
{
    uint8_t addr = 0x40;

    if (idx <= PCF8574_0x4E) {
        addr = 0x40 | (uint8_t)((idx - PCF8574_0x40) << 1);
    } else if (idx <= PCF8574A_0x7E) {
        addr = 0x70 | (uint8_t)((idx - PCF8574A_0x70) << 1);
    }

    return addr;
}

void i2cexpSend(I2cAddrIdx idx, uint8_t data)
{
    if (idx == I2C_ADDR_DISABLED || idx >= I2C_ADDR_END) {
        return;
    }

    uint8_t i2cAddr = i2cexpGetAddr(idx);

    i2cBegin(I2C_AMP, i2cAddr);
    i2cSend(I2C_AMP, data);
    i2cTransmit(I2C_AMP);
}
