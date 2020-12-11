#ifndef LC7213X_REGS_H
#define LC7213X_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

// IN1 mode control word bytes
#define LC7213X_IN1_SNS             0x01    // 0: AM 0.5..10 MHz, 1: AM 2..40 MHz
#define LC7213X_IN1_DVS             0x02    // 0: Select AM input, 1: Select FM input
#define LC7213X_IN1_CTE             0x04    // 0: IF Counter reset, 1: IF Counter start
#define LC7213X_IN1_XS              0x08    // 0: 4.5 MHz crystal, 1: 7.2 MHz crystal
#define LC7213X_IN1_FREF_MASK       0xF0    // Reference frequency selection mask
#define LC7213X_IN1_FREF_100K       0x00    // Reference freq 100kHz
#define LC7213X_IN1_FREF_50K        0x10    // Reference freq 50kHz
#define LC7213X_IN1_FREF_25K        0x20    // Reference freq 25kHz
#define LC7213X_IN1_FREF_25K_2      0x30    // Reference freq 25kHz
#define LC7213X_IN1_FREF_12K5       0x40    // Reference freq 12.5kHz
#define LC7213X_IN1_FREF_6K25       0x50    // Reference freq 6.25kHz
#define LC7213X_IN1_FREF_3K125      0x60    // Reference freq 3.125kHz
#define LC7213X_IN1_FREF_3K125_2    0x70    // Reference freq 3.125kHz
#define LC7213X_IN1_FREF_10K        0x80    // Reference freq 10kHz
#define LC7213X_IN1_FREF_9K         0x90    // Reference freq 9kHz
#define LC7213X_IN1_FREF_5K         0xA0    // Reference freq 5kHz
#define LC7213X_IN1_FREF_1K         0xB0    // Reference freq 1kHz
#define LC7213X_IN1_FREF_3K         0xC0    // Reference freq 3kHz
#define LC7213X_IN1_FREF_15K        0xD0    // Reference freq 15kHz
#define LC7213X_IN1_NO_PLL_NO_OSC   0xE0    // PLL inhibit + Xtal OSC stopped
#define LC7213X_IN1_NO_PLL          0xF0    // PLL inhibit

// IN2 mode control byte 0
#define LC7213X_IN2_IOC1            0x01    // 0 - Input mode, 1 - output mode on IO1
#define LC7213X_IN2_IOC2            0x02    // 0 - Input mode, 1 - output mode on IO2
#define LC7213X_IN2_IO1             0x04    // IO1 output value
#define LC7213X_IN2_IO2             0x08    // IO2 output value
#define LC7213X_IN2_BO1             0x10    // BO1 output value
#define LC7213X_IN2_BO2             0x20    // BO2 output value
#define LC7213X_IN2_BO3             0x40    // BO3 output value
#define LC7213X_IN2_BO4             0x80    // BO4 output value

// IN2 mode control byte 1
#define LC7213X_IN2_BO5             0x01    // BO5 output value
#define LC7213X_IN2_DOC_MASK        0x0E    // DO pin output control mask
#define LC7213X_IN2_DO_OPEN_1       0x00    // Open state
#define LC7213X_IN2_DO_UNLOCK       0x02    // Low when unlock state detected
#define LC7213X_IN2_DO_END_UC       0x04    // IF counter measure completion: 0: completed, 1: in progress
#define LC7213X_IN2_DO_OPEN_2       0x06    // Open state
#define LC7213X_IN2_DO_OPEN_3       0x08    // Open state
#define LC7213X_IN2_DO_IO1_STATE    0x0A    // Reflect IO1 pin state
#define LC7213X_IN2_DO_IO2_STATE    0x0C    // Reflect IO2 pin state
#define LC7213X_IN2_DO_OPEN_4       0x0E    // Open state
#define LC7213X_IN2_UL_MASK         0x30    // Unlock detection mask
#define LC7213X_IN2_UL_STOPPED      0x00    // Unlock detection stopped
#define LC7213X_IN2_UL_0            0x10    // Unlock detection 0: output directly
#define LC7213X_IN2_UL_0U55         0x20    // Unlock detection +-0.55us: extended by 1..2 ms
#define LC7213X_IN2_UL_1U11         0x30    // Unlock detection +-1.11us: extended by 1..2 ms
#define LC7213X_IN2_DZ_MASK         0x40    // Phase comparator dead zone mask
#define LC7213X_IN2_DZ_A            0x00    // Dead zone A
#define LC7213X_IN2_DZ_B            0x40    // Dead zone B
#define LC7213X_IN2_DZ_C            0x80    // Dead zone C
#define LC7213X_IN2_DZ_D            0xC0    // Dead zone D

// IN2 mode control byte 2
#define LC7213X_IN2_GT_MASK         0x03    // Control IF counter measurement period mask
#define LC7213X_IN2_GT_4M           0x00    // 4ms
#define LC7213X_IN2_GT_8M           0x01    // 8ms
#define LC7213X_IN2_GT_32M          0x02    // 32ms
#define LC7213X_IN2_GT_64M          0x03    // 64ms
#define LC7213X_IN2_TBC             0x04    // Clock time base - 0 - off, 1 - 8Hz on BO1
#define LC7213X_IN2_DLC             0x08    // Charge pump output - 0 - normal, 1 - forced low
#define LC7213X_IN2_IFS             0x10    // If counter control: 1 - normal mode, 0 - degradation mode
#define LC7213X_IN2_TEST_MASK       0xE0    // Should be 0

// OUT mode
#define LC7213X_OUT_I2              0x01    // I2 input status
#define LC7213X_OUT_I1              0x02    // I1 input status
#define LC7213X_OUT_UL              0x02    // 0: PLL unlocked, 1: PLL locked

#ifdef __cplusplus
}
#endif

#endif // LC7213X_REGS_H
