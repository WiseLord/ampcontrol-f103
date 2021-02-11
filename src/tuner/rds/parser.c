#include "parser.h"

#include <string.h>

#define RDS_VERSION_A           0
#define RDS_VERSION_B           1

#define RDS_A_PICODE_MASK       0xFFFF

#define RDS_B_GROUP_MASK        0xF000
#define RDS_B_GROUP_POS         12
#define RDS_B_VERSION_MASK      0x0800
#define RDS_B_VERSION_POS       11
#define RDS_B_TP_MASK           0x0400
#define RDS_B_TP_POS            10
#define RDS_B_PTY_MASK          0x03E0
#define RDS_B_PTY_POS           5

// Group 0: Basic tuning and switching information
#define RDS_B_TA_MASK           0x0010
#define RDS_B_TA_POS            4
#define RDS_B_MS_MASK           0x0008
#define RDS_B_MS_POS            3
#define RDS_B_DI_MASK           0x0004
#define RDS_B_DI_POS            2
#define RDS_B_PS_MASK           0x0003
#define RDS_B_PS_POS            0

// Group 2: Radiotext
#define RDS_B_TEXT_MASK         0x000F
#define RDS_B_TEXT_POS          0

static RdsParser parser;
static RdsParserCb rdsParserCb;

static void informReady(void)
{
    parser.flags |= RDS_FLAG_READY;

    if (rdsParserCb) {
        rdsParserCb();
    }
}

void rdsParserReset(void)
{
    memset(&parser, 0, sizeof (parser));
}

void rdsParserSetCb(RdsParserCb cb)
{
    rdsParserCb = cb;
}

static char convChar(char ch)
{
     return ch == 0x0D ? '\0' : ch == 0x0A ? ' ' : ch;
}

void rdsParserDecode(RdsBlock *block)
{
    parser.PI = block->a;

    uint8_t rdsGroup = (block->b >> RDS_B_GROUP_POS) & (RDS_B_GROUP_MASK >> RDS_B_GROUP_POS);
    uint8_t rdsVersion = (block->b >> RDS_B_VERSION_POS) & (RDS_B_VERSION_MASK >> RDS_B_VERSION_POS);

    parser.PTY = (block->b & RDS_B_PTY_MASK) >> RDS_B_PTY_POS;

    if (block->b & RDS_B_TP_MASK) {
        parser.flags |= RDS_FLAG_TP;
    } else {
        parser.flags &= ~RDS_FLAG_TP;
    }

    switch (rdsGroup) {
    case 0: {
        uint8_t PSN_index = (block->b >> RDS_B_PS_POS) & (RDS_B_PS_MASK >> RDS_B_PS_POS);

        bool DI = (block->b & RDS_B_DI_MASK) >> RDS_B_DI_POS;

        parser.PS[2 * PSN_index + 0] = (block->d >> 8) & 0x7F;
        parser.PS[2 * PSN_index + 1] = (block->d >> 0) & 0x7F;

        if (block->b & RDS_B_TP_MASK) {
            parser.flags |= RDS_FLAG_TA;
        } else {
            parser.flags &= ~RDS_FLAG_TA;
        }

        if (block->b & RDS_B_MS_MASK) {
            parser.flags |= RDS_FLAG_MS;
        } else {
            parser.flags &= ~RDS_FLAG_MS;
        }

        switch (PSN_index) {
        case 0:
            DI ? (parser.flags |= RDS_FLAG_DI_ST) : (parser.flags &= ~RDS_FLAG_DI_ST);
            break;
        case 1:
            DI ? (parser.flags |= RDS_FLAG_DI_AH) : (parser.flags &= ~RDS_FLAG_DI_AH);
            break;
        case 2:
            DI ? (parser.flags |= RDS_FLAG_DI_CMP) : (parser.flags &= ~RDS_FLAG_DI_CMP);
            break;
        case 3:
            DI ? (parser.flags |= RDS_FLAG_DI_STPTY) : (parser.flags &= ~RDS_FLAG_DI_STPTY);
            break;
        }
        informReady();
        break;
    }
    case 2: {
        uint8_t tIdx = (block->b >> RDS_B_TEXT_POS) & (RDS_B_TEXT_MASK >> RDS_B_TEXT_POS);
        char ch = 0;
        switch (rdsVersion) {
        case RDS_VERSION_A:
            ch = (block->c >> 8) & 0x7F;
            parser.text[4 * tIdx + 0] = convChar(ch);
            ch = (block->c >> 0) & 0x7F;
            parser.text[4 * tIdx + 1] = convChar(ch);
            ch = (block->d >> 8) & 0x7F;
            parser.text[4 * tIdx + 2] = convChar(ch);
            ch = (block->d >> 0) & 0x7F;
            parser.text[4 * tIdx + 3] = convChar(ch);
            break;
        case RDS_VERSION_B:
            ch = (block->d >> 8) & 0x7F;
            parser.text[2 * tIdx + 0] = convChar(ch);
            ch = (block->d >> 0) & 0x7F;
            parser.text[2 * tIdx + 1] = convChar(ch);
            break;
        }
        informReady();
        break;
    }
    }
}

RdsParser *rdsParserGet()
{
    return &parser;
}
