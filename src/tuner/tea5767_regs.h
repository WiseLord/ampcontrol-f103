#ifndef TEA5767_REGS_H
#define TEA5767_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

// Write mode

// 0 register
#define TEA5767_MUTE            0x80 // Mute output (1)
#define TEA5767_SM              0x40 // Search mode (1)
#define TEA5767_WR_PLL_13_8     0x3F // Write PLL regs 13..8

// 1 register
#define TEA5767_WR_PLL_7_0      0xFF // Write PLL regs 7..0

// 2 register
#define TEA5767_SUD             0x80 // Search Up(1) / Down(0)
#define TEA5767_SSL             0x60 // Search stop level
#define TEA5767_SSL_LOW         0x20 // Low: 5
#define TEA5767_SSL_MID         0x40 // Middle: 7
#define TEA5767_SSL_HIGH        0x60 // High: 10
#define TEA5767_HLSI            0x10 // High(1) / Low(0) Side Injection
#define TEA5767_MS              0x08 // Mono to stereo
#define TEA5767_MR              0x04 // Mute Right
#define TEA5767_ML              0x02 // Mute Left
#define TEA5767_SWP1            0x01 // Software programmable port 1 high

// 3 register
#define TEA5767_SWP2            0x80 // Software programmable port 2 high
#define TEA5767_STBY            0x40 // Standby
#define TEA5767_BL              0x20 // Band limit: Japan(1) / Europe(0)
#define TEA5767_XTAL            0x10 // Clock frequency 32768 (1)
#define TEA5767_SMUTE           0x08 // Soft mute
#define TEA5767_HCC             0x04 // High cut control
#define TEA5767_SNC             0x02 // Stereo Noise Cancelling
#define TEA5767_SI              0x01 // Search indicator

// 4 register
#define TEA5767_PLLREF          0x80 // 6.5MHz reference frequency
#define TEA5767_DTC             0x40 // De-emphasis 75us(1) / 50us(0)
// Not used 5..0 bits

// Read mode

// 0 register
#define TEA5767_RF              0x80 // Ready flag
#define TEA5767_BLF             0x40 // Band limit flag
#define TEA5767_RD_PLL_13_8     0x3F // Read PLL regs 13..8

// 1 register
#define TEA5767_RD_PLL_7_0      0xFF // Read PLL regs 7..0

// 2 register
#define TEA5767_STEREO          0x80 // Stereo reception
#define TEA5767_IF_CNT          0x7F // If counter result

// 3 register
#define TEA5767_LEV             0xF0 // ADC Level
#define TEA5767_CI              0x0F // Chip ID(0000)

// 4 register
#define TEA5767_RESERVED_MASK   0xFF // Not used

#ifdef __cplusplus
}
#endif

#endif // TEA5767_REGS_H
