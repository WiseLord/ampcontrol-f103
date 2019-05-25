#ifndef QN8006_REGS_H
#define QN8006_REGS_H

/*
#define QN8006_      0x80
#define QN8006_      0x40
#define QN8006_      0x20
#define QN8006_      0x10
#define QN8006_      0x08
#define QN8006_      0x04
#define QN8006_      0x02
#define QN8006_      0x01
*/

// 00: SYSTEM1
#define QN8006_RXREQ        0x80
#define QN8006_TXREQ        0x40
#define QN8006_CHSC         0x20    // 1: scan mode
#define QN8006_STNBY        0x10
#define QN8006_RXI2S        0x08
#define QN8006_TXI2S        0x04
#define QN8006_RDSEN        0x02
#define QN8006_CCA_CH_DIS   0x01

// 01: SYSTEM2
#define QN8006_SWRST        0x80
#define QN8006_RECAL        0x40
#define QN8006_FORCE_MO     0x20
#define QN8006_ST_MO_TX     0x10
#define QN8006_TC           0x08
#define QN8006_RDSTXRDY     0x04
#define QN8006_TMOUT        0x03

// 02: DEV_ADD
#define QN8006_RCXCCAD_5    0x80
#define QN8006_DADD         0x7F

// 03: ANACTL1
#define QN8006_MUTE_EN      0x80
#define QN8006_I2S_SCKINV   0x40
#define QN8006_RSTB_BB      0x20
#define QN8006_ANT_SEL      0x10
#define QN8006_XSEL         0x0F
#define QN8006_XSEL_26M     0x0B

// 04: REG_VGA
#define QN8006_RIN          0xC0
#define QN8006_SCSEL        0x7F

// 05: CIDR1
// 06: CIDR2

// 07: I2S

// 08: CH
#define QN8006_CH_7_0       0xFF

// 09: CH_START
#define QN8006_CH_STA_7_0   0xFF

// 0A: CH_STOP
#define QN8006_CH_STP_7_0   0xFF

// 0B: CH_STEP
#define QN8006_FSTEP        0xC0
#define QN8006_FSTEP_50K    0x00
#define QN8006_FSTEP_100K   0x40
#define QN8006_FSTEP_200K   0x80

#define QN8006_CH_STP_9_8   0x30
#define QN8006_CH_STA_9_8   0x0C
#define QN8006_CH_9_8       0x03

// 0C: PAC_TARGET
// 0D: TXAGC_GAIN
// 0E: TX_FDEV
// 0F: GAIN_TXPLT

// 10: RDSD0
// 11: RDSD1
// 12: RDSD2
// 13: RDSD3
// 14: RDSD4
// 15: RDSD5
// 16: RDSD6
// 17: RDSD7

// 18: RDSFDEV
#define QN8006_RDS_ONLY     0x80
#define QN8006_RDSFDEV      0x7F

// 19: CCA
#define QN8006_TXCCA_2_0    0xE0
#define QN8006_RXCCAD_4_0   0x1F

// 1A: STATUS1
#define QN8006_RXCCA_FAIL   0x40
#define QN8006_I2SOVFL      0x20
#define QN8006_i2SUNDFL     0x10
#define QN8006_INSAT        0x08
#define QN8006_RXAGCSET     0x04
#define QN8006_RXAGCERR     0x02
#define QN8006_ST_MO_RX     0x01 // 1: mono

// 1B: STATUS3
#define QN8006_RDS_RXTXUPD  0x80
#define QN8006_E_DET        0x40
#define QN8006_RDSC0C1      0x20
#define QN8006_RDSSYNC      0x10
#define QN8006_RDS0ERR      0x08
#define QN8006_RDS1ERR      0x04
#define QN8006_RDS2ERR      0x02
#define QN8006_RDS3ERR      0x01

// 1C: RSSISIG
#define QN8006_RSSIDB       0xFF

// 21: RSSIMP
#define QN8006_RSSIMPDB     0x7F

// 22: SNR
#define QN8006_SNRDB        0xFF

// 49: REG_XLT3
#define QN8006_XTLBYP       0x10

// 4F: REG_DAC
#define QN8006_DACG         0x03

// 59: PAC_CAL
// 5A: PAG_CAL


#endif // QN8006_REGS_H
