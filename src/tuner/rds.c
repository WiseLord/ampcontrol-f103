#include "rds.h"

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

static Rds rds;

void rdsReset()
{
    memset(&rds, 0, sizeof (rds));
    memset(&rds.PS, ' ', 8);
    memset(&rds.text, ' ', 64);
}

void rdsDecode(RdsBlock *block)
{
    rds.PI = block->a;

    uint8_t rdsGroup = (block->b >> RDS_B_GROUP_POS) & (RDS_B_GROUP_MASK >> RDS_B_GROUP_POS);
    uint8_t rdsVersion = (block->b >> RDS_B_VERSION_POS) & (RDS_B_VERSION_MASK >> RDS_B_VERSION_POS);

    rds.TP = (block->b >> RDS_B_TP_POS) & (RDS_B_TP_MASK >> RDS_B_TP_POS);
    rds.PTY = (block->b >> RDS_B_PTY_POS) & (RDS_B_PTY_MASK >> RDS_B_PTY_POS);

    switch (rdsGroup) {
    case 0: {
        uint8_t PSN_index = (block->b >> RDS_B_PS_POS) & (RDS_B_PS_MASK >> RDS_B_PS_POS);
        bool DI = (block->b >> RDS_B_DI_POS) & (RDS_B_DI_MASK >> RDS_B_DI_POS);

        rds.PS[2 * PSN_index + 0] = (block->d >> 8) & 0x7F;
        rds.PS[2 * PSN_index + 1] = (block->d >> 0) & 0x7F;

        rds.TA = (block->b >> RDS_B_TA_POS) & (RDS_B_TP_MASK >> RDS_B_TA_POS);
        rds.MS = (block->b >> RDS_B_MS_POS) & (RDS_B_MS_MASK >> RDS_B_MS_POS);

        switch (PSN_index) {
        case 0:
            rds.DI_ST = DI;
            break;
        case 1:
            rds.DI_AH = DI;
            break;
        case 2:
            rds.DI_CMP = DI;
            break;
        case 3:
            rds.DI_stPTY = DI;
            break;
        }
        break;
    }
    case 2: {
        uint8_t text_index = (block->b >> RDS_B_TEXT_POS) & (RDS_B_TEXT_MASK >> RDS_B_TEXT_POS);
        switch (rdsVersion) {
        case RDS_VERSION_A:
            rds.text[4 * text_index + 0] = (block->c >> 8) & 0x7F;
            rds.text[4 * text_index + 1] = (block->c >> 0) & 0x7F;
            rds.text[4 * text_index + 2] = (block->d >> 8) & 0x7F;
            rds.text[4 * text_index + 3] = (block->d >> 0) & 0x7F;
            break;
        case RDS_VERSION_B:
            rds.text[2 * text_index + 0] = (block->d >> 8) & 0x7F;
            rds.text[2 * text_index + 1] = (block->d >> 0) & 0x7F;
            break;
        }
    }
    }
}

Rds *rdsGet()
{
    return &rds;
}
