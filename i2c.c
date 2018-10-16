#include "i2c.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_i2c.h>

#define I2C_SEND_TIMEOUT_TXE_MS              5
#define I2C_SEND_TIMEOUT_RXE_MS              5
#define I2C_SEND_TIMEOUT_SB_MS               5
#define I2C_SEND_TIMEOUT_ADDR_MS             5

static uint8_t i2cBuf[I2C_BUF_SIZE];
static uint8_t i2cBytes = 0;
static uint8_t i2cAddr = 0;
static uint32_t i2cTimeout = 0;

uint8_t i2cInit(I2C_TypeDef *I2Cx, uint32_t ClockSpeed)
{
    if (I2Cx == I2C_AMP) {
        LL_APB1_GRP1_EnableClock(I2C_AMP_PERIF);
    } else {
        LL_APB1_GRP1_EnableClock(I2C_LCD_PERIF);
    }

    LL_I2C_Disable(I2Cx);

    LL_I2C_DisableOwnAddress2(I2Cx);
    LL_I2C_DisableGeneralCall(I2Cx);
    LL_I2C_EnableClockStretching(I2Cx);

    LL_I2C_InitTypeDef I2C_InitStruct;
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
    I2C_InitStruct.ClockSpeed = ClockSpeed;
    I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_16_9;
    I2C_InitStruct.OwnAddress1 = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2Cx, &I2C_InitStruct);

    LL_I2C_SetOwnAddress2(I2Cx, 0);

    LL_I2C_Enable(I2Cx);

    return 0;
}

uint8_t i2cStart(I2C_TypeDef *I2Cx, uint8_t addr)
{
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);

    LL_I2C_GenerateStartCondition(I2Cx);

    i2cTimeout = I2C_SEND_TIMEOUT_SB_MS;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx)) {

    }

    LL_I2C_TransmitData8(I2Cx, addr | I2C_WRITE);

    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx)) {

    }

    LL_I2C_ClearFlag_ADDR(I2Cx);

    return 0;
}

uint8_t i2cStop(I2C_TypeDef *I2Cx)
{
    while (!LL_I2C_IsActiveFlag_TXE(I2Cx));

    LL_I2C_GenerateStopCondition(I2Cx);

    return 0;
}

uint8_t i2cWrite(I2C_TypeDef *I2Cx, uint8_t data)
{
    while (!LL_I2C_IsActiveFlag_TXE(I2Cx));

    LL_I2C_TransmitData8(I2Cx, data);

    return 0;
}


void i2cBegin(I2C_TypeDef *I2Cx, uint8_t addr)
{
    i2cBytes = 0;
    i2cAddr = addr;
}

void i2cSend(I2C_TypeDef *I2Cx, uint8_t data)
{
    i2cBuf[i2cBytes++] = data;
}

void i2cTransmit(I2C_TypeDef *I2Cx)
{
    // Handle start
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
    LL_I2C_GenerateStartCondition(I2Cx);

    i2cTimeout = I2C_SEND_TIMEOUT_SB_MS;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx)) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (i2cTimeout-- == 0) {
                // TODO: handle error
                return;
            }
        }
    }

    LL_I2C_TransmitData8(I2Cx, i2cAddr | I2C_WRITE);

    i2cTimeout = I2C_SEND_TIMEOUT_ADDR_MS;
    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx)) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (i2cTimeout-- == 0) {
                // TODO: handle error
                return;
            }
        }
    }

    LL_I2C_ClearFlag_ADDR(I2Cx);

    for (uint8_t i = 0; i < i2cBytes; i++) {

        i2cTimeout = I2C_SEND_TIMEOUT_TXE_MS;
        while (!LL_I2C_IsActiveFlag_TXE(I2Cx)) {
            if (LL_SYSTICK_IsActiveCounterFlag()) {
                if (i2cTimeout-- == 0) {
                    // TODO: handle error
                    return;
                }
            }
        }

        LL_I2C_TransmitData8(I2Cx, i2cBuf[i]);
    }

    while (!LL_I2C_IsActiveFlag_TXE(I2Cx)) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (i2cTimeout-- == 0) {
                // TODO: handle error
                return;
            }
        }
    }

    LL_I2C_GenerateStopCondition(I2Cx);
}

void i2cReceive(I2C_TypeDef *I2Cx, uint8_t *buf, uint8_t size)
{
//    if (size == 1) {
//        // Send START condition
//        LL_I2C_GenerateStartCondition(I2Cx);

//        i2cTimeout = I2C_SEND_TIMEOUT_SB_MS;
//        while (!LL_I2C_IsActiveFlag_SB(I2Cx)) {
//            if (LL_SYSTICK_IsActiveCounterFlag()) {
//                if (i2cTimeout-- == 0) {
//                    // TODO: handle error
//                    return;
//                }
//            }
//        }

//        // Send slave address
//        LL_I2C_TransmitData8(I2Cx, i2cAddr | I2C_READ);

//        i2cTimeout = I2C_SEND_TIMEOUT_ADDR_MS;
//        while (!LL_I2C_IsActiveFlag_ADDR(I2Cx)) {
//            if (LL_SYSTICK_IsActiveCounterFlag()) {
//                if (i2cTimeout-- == 0) {
//                    // TODO: handle error
////                    return;
//                }
//            }
//        }
//        // Clear ACK bit
//        LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);

//        __disable_irq();
//        LL_I2C_ClearFlag_ADDR(I2Cx);
//        LL_I2C_GenerateStopCondition(I2Cx);
//        __enable_irq();

//        i2cTimeout = I2C_SEND_TIMEOUT_RXE_MS;
//        // Wait until a data is received in DR register (RXNE = 1) EV7
//        while(!LL_I2C_IsActiveFlag_RXNE(I2Cx)) {
//            if (LL_SYSTICK_IsActiveCounterFlag()) {
//                if (i2cTimeout-- == 0) {
//                    // TODO: handle error
////                    return;
//                }
//            }
//        }
//        // Read the data
//        *buf = LL_I2C_ReceiveData8(I2Cx);

//        i2cTimeout = I2C_SEND_TIMEOUT_RXE_MS;
//        while(LL_I2C_IsActiveFlag_STOP(I2Cx)) {
//            if (LL_SYSTICK_IsActiveCounterFlag()) {
//                if (i2cTimeout-- == 0) {
//                    // TODO: handle error
////                    return;
//                }
//            }
//        }

//        // Enable Acknowledgement to be ready for another reception
//        LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
//    }


    LL_I2C_GenerateStartCondition(I2Cx);

    i2cTimeout = I2C_SEND_TIMEOUT_SB_MS;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx)) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (i2cTimeout-- == 0) {
                // TODO: handle error
                return;
            }
        }
    }

    //Send device address again + read
    LL_I2C_TransmitData8(I2Cx, i2cAddr | I2C_READ);

    i2cTimeout = I2C_SEND_TIMEOUT_ADDR_MS;
    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx)) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (i2cTimeout-- == 0) {
                // TODO: handle error
                return;
            }
        }
    }

    //Read out data MSB, send ACK
    LL_I2C_ClearFlag_ADDR(I2Cx);
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);

    i2cTimeout = I2C_SEND_TIMEOUT_RXE_MS;
    while (!LL_I2C_IsActiveFlag_RXNE(I2Cx)) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (i2cTimeout-- == 0) {
                // TODO: handle error
                return;
            }
        }
    }
    *buf++ = LL_I2C_ReceiveData8(I2Cx);

    //Read out data LSB, send NACK
    i2cTimeout = I2C_SEND_TIMEOUT_RXE_MS;
    while (!LL_I2C_IsActiveFlag_RXNE(I2Cx)) {
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            if (i2cTimeout-- == 0) {
                // TODO: handle error
                return;
            }
        }
    }
    *buf++ = LL_I2C_ReceiveData8(I2Cx);
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);

    LL_I2C_GenerateStopCondition(I2Cx);
}

#ifdef HIDE
void I2C_Master_BufferRead(I2C_TypeDef *I2Cx, uint8_t *pBuffer,  uint32_t NumByteToRead,
                           uint8_t SlaveAddress)
{
    __IO uint32_t temp = 0;
    __IO uint32_t Timeout = 0;

    /* Enable I2C errors interrupts (used in all modes: Polling, DMA and Interrupts */
    I2Cx->CR2 |= I2C_IT_ERR;

    if (NumByteToRead == 1) {
        Timeout = 0xFFFF;
        /* Send START condition */
        I2Cx->CR1 |= CR1_START_Set;
        /* Wait until SB flag is set: EV5  */
        while ((I2Cx->SR1 & 0x0001) != 0x0001) {
            if (Timeout-- == 0)
                return Error;
        }
        /* Send slave address */
        /* Reset the address bit0 for read */
        SlaveAddress |= OAR1_ADD0_Set;
        Address = SlaveAddress;
        /* Send the slave address */
        I2Cx->DR = Address;
        /* Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
        and program the STOP just after ADDR is cleared. The EV6_3
        software sequence must complete before the current byte end of transfer.*/
        /* Wait until ADDR is set */
        Timeout = 0xFFFF;
        while ((I2Cx->SR1 & 0x0002) != 0x0002) {
            if (Timeout-- == 0)
                return Error;
        }
        /* Clear ACK bit */
        I2Cx->CR1 &= CR1_ACK_Reset;
        /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
        software sequence must complete before the current byte end of transfer */
        __disable_irq();
        /* Clear ADDR flag */
        temp = I2Cx->SR2;
        /* Program the STOP */
        I2Cx->CR1 |= CR1_STOP_Set;
        /* Re-enable IRQs */
        __enable_irq();
        /* Wait until a data is received in DR register (RXNE = 1) EV7 */
        while ((I2Cx->SR1 & 0x00040) != 0x000040);
        /* Read the data */
        *pBuffer = I2Cx->DR;
        /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
        while ((I2Cx->CR1 & 0x200) == 0x200);
        /* Enable Acknowledgement to be ready for another reception */
        I2Cx->CR1 |= CR1_ACK_Set;

    }

    else if (NumByteToRead == 2) {
        /* Set POS bit */
        I2Cx->CR1 |= CR1_POS_Set;
        Timeout = 0xFFFF;
        /* Send START condition */
        I2Cx->CR1 |= CR1_START_Set;
        /* Wait until SB flag is set: EV5 */
        while ((I2Cx->SR1 & 0x0001) != 0x0001) {
            if (Timeout-- == 0)
                return Error;
        }
        Timeout = 0xFFFF;
        /* Send slave address */
        /* Set the address bit0 for read */
        SlaveAddress |= OAR1_ADD0_Set;
        Address = SlaveAddress;
        /* Send the slave address */
        I2Cx->DR = Address;
        /* Wait until ADDR is set: EV6 */
        while ((I2Cx->SR1 & 0x0002) != 0x0002) {
            if (Timeout-- == 0)
                return Error;
        }
        /* EV6_1: The acknowledge disable should be done just after EV6,
        that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and
        ACK clearing */
        __disable_irq();
        /* Clear ADDR by reading SR2 register  */
        temp = I2Cx->SR2;
        /* Clear ACK */
        I2Cx->CR1 &= CR1_ACK_Reset;
        /*Re-enable IRQs */
        __enable_irq();
        /* Wait until BTF is set */
        while ((I2Cx->SR1 & 0x00004) != 0x000004);
        /* Disable IRQs around STOP programming and data reading because of the limitation ?*/
        __disable_irq();
        /* Program the STOP */
        I2C_GenerateSTOP(I2Cx, ENABLE);
        /* Read first data */
        *pBuffer = I2Cx->DR;
        /* Re-enable IRQs */
        __enable_irq();
        /**/
        pBuffer++;
        /* Read second data */
        *pBuffer = I2Cx->DR;
        /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
        while ((I2Cx->CR1 & 0x200) == 0x200);
        /* Enable Acknowledgement to be ready for another reception */
        I2Cx->CR1  |= CR1_ACK_Set;
        /* Clear POS bit */
        I2Cx->CR1  &= CR1_POS_Reset;

    }

    else

    {

        Timeout = 0xFFFF;
        /* Send START condition */
        I2Cx->CR1 |= CR1_START_Set;
        /* Wait until SB flag is set: EV5 */
        while ((I2Cx->SR1 & 0x0001) != 0x0001) {
            if (Timeout-- == 0)
                return Error;
        }
        Timeout = 0xFFFF;
        /* Send slave address */
        /* Reset the address bit0 for write */
        SlaveAddress |= OAR1_ADD0_Set;;
        Address = SlaveAddress;
        /* Send the slave address */
        I2Cx->DR = Address;
        /* Wait until ADDR is set: EV6 */
        while ((I2Cx->SR1 & 0x0002) != 0x0002) {
            if (Timeout-- == 0)
                return Error;
        }
        /* Clear ADDR by reading SR2 status register */
        temp = I2Cx->SR2;
        /* While there is data to be read */
        while (NumByteToRead) {
            /* Receive bytes from first byte until byte N-3 */
            if (NumByteToRead != 3) {
                /* Poll on BTF to receive data because in polling mode we can not guarantee the
                EV7 software sequence is managed before the current byte transfer completes */
                while ((I2Cx->SR1 & 0x00004) != 0x000004);
                /* Read data */
                *pBuffer = I2Cx->DR;
                /* */
                pBuffer++;
                /* Decrement the read bytes counter */
                NumByteToRead--;
            }

            /* it remains to read three data: data N-2, data N-1, Data N */
            if (NumByteToRead == 3) {

                /* Wait until BTF is set: Data N-2 in DR and data N -1 in shift register */
                while ((I2Cx->SR1 & 0x00004) != 0x000004);
                /* Clear ACK */
                I2Cx->CR1 &= CR1_ACK_Reset;

                /* Disable IRQs around data reading and STOP programming because of the
                limitation ? */
                __disable_irq();
                /* Read Data N-2 */
                *pBuffer = I2Cx->DR;
                /* Increment */
                pBuffer++;
                /* Program the STOP */
                I2Cx->CR1 |= CR1_STOP_Set;
                /* Read DataN-1 */
                *pBuffer = I2Cx->DR;
                /* Re-enable IRQs */
                __enable_irq();
                /* Increment */
                pBuffer++;
                /* Wait until RXNE is set (DR contains the last data) */
                while ((I2Cx->SR1 & 0x00040) != 0x000040);
                /* Read DataN */
                *pBuffer = I2Cx->DR;
                /* Reset the number of bytes to be read by master */
                NumByteToRead = 0;

            }
        }
        /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
        while ((I2Cx->CR1 & 0x200) == 0x200);
        /* Enable Acknowledgement to be ready for another reception */
        I2Cx->CR1 |= CR1_ACK_Set;

    }


    return Success;
}



uint16_t MLX90640_I2CReadWord(uint8_t slave_address, uint16_t start_address) {

    uint16_t puff;
    volatile uint8_t reg_m,reg_l,dat_m,dat_l;

    reg_m = (uint8_t) ((start_address & 0xFF00) >> 8);	//Address MSB
    reg_l = (uint8_t) (start_address & 0x00FF); 	    //Address LSB


    while (LL_I2C_IsActiveFlag_BUSY(I2C1)) {
    }

    //LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
    LL_I2C_GenerateStartCondition(I2C1);
    while (!LL_I2C_IsActiveFlag_SB(I2C1)) {
    }

    //Send  device address
    LL_I2C_TransmitData8(I2C1, slave_address);
    while (!LL_I2C_IsActiveFlag_ADDR(I2C1)) {
    }
    LL_I2C_ClearFlag_ADDR(I2C1);

    while (!LL_I2C_IsActiveFlag_TXE(I2C1)) {
    }

    //Send start address MSB
    LL_I2C_TransmitData8(I2C1, reg_m);
    while (!LL_I2C_IsActiveFlag_TXE(I2C1)) {
    }

    //Send start address LSB
    LL_I2C_TransmitData8(I2C1, reg_l);
    while (!LL_I2C_IsActiveFlag_TXE(I2C1)) {
    }

    //Repeat start condition
    LL_I2C_GenerateStartCondition(I2C1);
    while (!LL_I2C_IsActiveFlag_SB(I2C1)) {
    }

    //Send device address again + read
    LL_I2C_TransmitData8(I2C1, slave_address | 0x01);
    while (!LL_I2C_IsActiveFlag_ADDR(I2C1)) {
    }

    //Read out data MSB, send ACK
    LL_I2C_ClearFlag_ADDR(I2C1);
    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
    while (!LL_I2C_IsActiveFlag_RXNE(I2C1)) {
    }
    dat_m = LL_I2C_ReceiveData8(I2C1);

    //Read out data LSB, send NACK
    while (!LL_I2C_IsActiveFlag_RXNE(I2C1)) {
    }
    dat_l = LL_I2C_ReceiveData8(I2C1);
    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
    LL_I2C_GenerateStopCondition(I2C1);

    return ((uint16_t) (dat_m << 8)) | ((uint16_t)((dat_l) & 0x00FF));

}
#endif
