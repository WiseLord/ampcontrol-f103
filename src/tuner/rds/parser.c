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

void rdsParserReset()
{
    memset(&parser, 0, sizeof (parser));
    memset(&parser.PS, ' ', 8);
    memset(&parser.text, ' ', 64);
}

void rdsParserDecode(RdsBlock *block)
{
    parser.PI = block->a;

    uint8_t rdsGroup = (block->b >> RDS_B_GROUP_POS) & (RDS_B_GROUP_MASK >> RDS_B_GROUP_POS);
    uint8_t rdsVersion = (block->b >> RDS_B_VERSION_POS) & (RDS_B_VERSION_MASK >> RDS_B_VERSION_POS);

    parser.TP = (block->b >> RDS_B_TP_POS) & (RDS_B_TP_MASK >> RDS_B_TP_POS);
    parser.PTY = (block->b >> RDS_B_PTY_POS) & (RDS_B_PTY_MASK >> RDS_B_PTY_POS);

    switch (rdsGroup) {
    case 0: {
        uint8_t PSN_index = (block->b >> RDS_B_PS_POS) & (RDS_B_PS_MASK >> RDS_B_PS_POS);
        bool DI = (block->b >> RDS_B_DI_POS) & (RDS_B_DI_MASK >> RDS_B_DI_POS);

        parser.PS[2 * PSN_index + 0] = (block->d >> 8) & 0x7F;
        parser.PS[2 * PSN_index + 1] = (block->d >> 0) & 0x7F;

        parser.TA = (block->b >> RDS_B_TA_POS) & (RDS_B_TP_MASK >> RDS_B_TA_POS);
        parser.MS = (block->b >> RDS_B_MS_POS) & (RDS_B_MS_MASK >> RDS_B_MS_POS);

        switch (PSN_index) {
        case 0:
            parser.DI_ST = DI;
            break;
        case 1:
            parser.DI_AH = DI;
            break;
        case 2:
            parser.DI_CMP = DI;
            break;
        case 3:
            parser.DI_stPTY = DI;
            break;
        }
        break;
    }
    case 2: {
        uint8_t text_index = (block->b >> RDS_B_TEXT_POS) & (RDS_B_TEXT_MASK >> RDS_B_TEXT_POS);
        switch (rdsVersion) {
        case RDS_VERSION_A:
            parser.text[4 * text_index + 0] = (block->c >> 8) & 0x7F;
            parser.text[4 * text_index + 1] = (block->c >> 0) & 0x7F;
            parser.text[4 * text_index + 2] = (block->d >> 8) & 0x7F;
            parser.text[4 * text_index + 3] = (block->d >> 0) & 0x7F;
            break;
        case RDS_VERSION_B:
            parser.text[2 * text_index + 0] = (block->d >> 8) & 0x7F;
            parser.text[2 * text_index + 1] = (block->d >> 0) & 0x7F;
            break;
        }
    }
    }
}

RdsParser *rdsParserGet()
{
    return &parser;
}
