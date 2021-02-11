#include "decoder.h"

#include <string.h>

#define OFT_NUM 5
static const uint16_t offset_word[OFT_NUM] = {
    0x0FC, 0x198, 0x168, 0x1B4, 0x350
};

static RdsDecoder decoder;

uint32_t calcSyndrome(uint32_t msg, uint32_t init)
{
    // x^10 + x^8 + x^7 + x^5 + x^4 + x^3 + 1;
    const uint32_t genPol = 0x5B9; // 0b10110111001

    const int8_t msgLen = 26;
    const int8_t chkOft = 10;

    uint32_t reg = 0;

    for (int8_t i = msgLen; i > 0; i--) {
        if ((msg >> (i - 1)) & 0x01) {
            reg = (reg << 1) ^ init;
        } else {
            reg = (reg << 1);
        }

        if (reg & (0x01 << chkOft)) {
            reg = reg ^ genPol;
        }
    }

    return (reg);
}

uint32_t correct(uint32_t msg)
{
    // x^9 + x^8 + x^4 + x^3 + x + 1
    uint32_t prem = 0x31B; // 0b1100011011

    const uint32_t trap = 0x1F;
    uint32_t reg = calcSyndrome(msg, prem);

    decoder.corrected = 0;
    for (int8_t dec = 16; dec >= 0; dec--) {
        reg = calcSyndrome(reg, 1);
        if ((reg & trap) == 0) {
            decoder.corrected = 1;
            reg = reg << dec;
            break;
        }
        reg = (reg << 1);
    }
    reg = msg ^ reg;

    return reg;
}

void rdsDecoderPushBit(bool data)
{
    decoder.raw <<= 1;
    decoder.raw |= (data ? 0x01 : 0x00);
    decoder.raw &= 0x03FFFFFF;

    if (!decoder.synced) {
        decoder.currIdx = 0; // Searching for block A
        decoder.syndrome = calcSyndrome(decoder.raw ^ offset_word[decoder.currIdx], 0);

        if (decoder.syndrome == 0) {
            // Found block A
            decoder.synced = true;
            decoder.block.blk[decoder.currIdx] = decoder.raw >> 10;
            decoder.goodMask = (1 << decoder.currIdx);
            decoder.goodCnt = 2; // Number of good consecutive groups to start parsing

            decoder.currCnt = 0;
            decoder.currIdx = 1; // Block B will be next
        } else {
            decoder.synced = false;
        }
    } else {
        if (++decoder.currCnt >= 26) {
            decoder.currCnt = 0;
            decoder.syndrome = correct(decoder.raw ^ offset_word[decoder.currIdx]);

            if (decoder.corrected) {
                decoder.block.blk[decoder.currIdx] = decoder.raw >> 10;
                decoder.goodMask |= (1 << decoder.currIdx);
            } else {
                decoder.synced = false;
            }

            if (decoder.currIdx == 3) {
                if (decoder.goodMask == 0x0F) {
                    if (decoder.goodCnt) {
                        decoder.goodCnt--;
                    }
                    if (decoder.goodCnt == 0) {
                        rdsParserDecode(&decoder.block);
                    }
                }
            }

            decoder.currIdx = (decoder.currIdx + 1) % 4;
            if (decoder.currIdx == 0) {
                decoder.goodMask = 0x00;
            }
        }
    }
}

RdsDecoder *rdsDecoderGet(void)
{
    return &decoder;
}
