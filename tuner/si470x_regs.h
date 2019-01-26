#ifndef SI470X_REGS_H
#define SI470X_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

// Write mode

// 0 byte (02H)
#define SI470X_DSMUTE               0x80 // Softmute enabled (0) / disabled (1)
#define SI470X_DMUTE                0x40 // Mute enabled (0) / disabled (1)
#define SI470X_MONO                 0x20 // Mono mode (1) / stereo mode (0);
#define SI470X_RDSM                 0x08 // RDS mode standard (0) / verbose (1)
#define SI470X_SKMODE               0x04 // Stop seeking on band limit (1)
#define SI470X_SEEKUP               0x02 // Seek up (1) / down (0)
#define SI470X_SEEK                 0x01 // Seek enabled (1) / disabled (0)

// 1 byte (02L)
#define SI470X_DISABLE              0x40 // Powerup disable
#define SI470X_ENABLE               0x01 // Powerup enable

// 2 byte (03H)
#define SI470X_TUNE                 0x80 // Tune enable (1)
#define SI470X_CHAN_9_8             0x03 // CHAN 9..8 bits

// 3 byte (03L)
#define SI470X_CHAN_7_0             0xFF // CHAN 7..0 bits

// 4 byte (04H)
#define SI470X_RDSIEN               0x80 // RDS interrup enable (1) (5ms low pulse on GPIO2 when RDSR is set)
#define SI470X_STCIEN               0x40 // Seek/tune complete interrupt enable (1) (5ms low pulse on GPIO2 when CTC is set)
#define SI470X_RDS                  0x10 // RDS enable (1)
#define SI470X_DE                   0x08 // De-emphasis 75us (0) / 50us (1)
#define SI470X_AGCD                 0x04 // AGC disable (1)

// 5 byte (04L)
#define SI470X_BLNDADJ              0xC0 // Stereo/Mono blend level
#define SI470X_BLNDADJ_31_49        0x00 // 31..49 RSSI dBuV (default)
#define SI470X_BLNDADJ_37_55        0x40 // 37..55 RSSI dBuV (+6 dB)
#define SI470X_BLNDADJ_19_37        0x80 // 19..37 RSSI dBuV (-12 dB)
#define SI470X_BLNDADJ_25_43        0xC0 // 25..43 RSSI dBuV (-6 dB)
#define SI470X_GPIO3                0x30 // GPIO3 control
#define SI470X_GPIO3_HI_IMP         0x00 // GPIO3 high impedance
#define SI470X_GPIO3_ST_IND         0x10 // GPIO3 as stereo indicator
#define SI470X_GPIO3_LO             0x20 // GPIO3 low level
#define SI470X_GPIO3_HI             0x30 // GPIO3 high level
#define SI470X_GPIO2                0x0C // GPIO2 control
#define SI470X_GPIO2_HI_IMP         0x00 // GPIO2 high impedance
#define SI470X_GPIO2_INT            0x04 // GPIO2 interrupt (INT)
#define SI470X_GPIO2_LO             0x08 // GPIO2 low level
#define SI470X_GPIO2_HI             0x0C // GPIO2 high level
#define SI470X_GPIO1                0x03 // GPIO1 control
#define SI470X_GPIO1_HI_IMP         0x00 // GPIO1 high impedance
#define SI470X_GPIO1_RSRVD          0x04 // GPIO1 reserved
#define SI470X_GPIO1_LO             0x08 // GPIO1 low level
#define SI470X_GPIO1_HI             0x0C // GPIO1 high level

// 6 byte (05H)
#define SI470X_SEEKTH               0xFF // RSSI seek threshold (default min = 0)

// 7 byte (05L)
#define SI470X_BAND                 0xC0 // Tuner band selection
#define SI470X_BAND_US_EUROPE       0x00 // 87.5..108 MHz
#define SI470X_BAND_JAPAN_WIDE      0x40 // 76..108 MHz
#define SI470X_BAND_JAPAN           0x80 // 76..90 MHz
#define SI470X_SPACE                0x30 // Channel spacing
#define SI470X_SPACE_200            0x00 // 200kHz step
#define SI470X_SPACE_100            0x10 // 100kHz step
#define SI470X_SPACE_50             0x20 // 50kHz step
#define SI470X_VOLUME               0x0F // 4 bits volume (0000 - muted, 1111 - max)

// 8 byte (06H)
#define SI470X_SMUTER               0xC0 // Softmute attack/recover Rate
#define SI470X_SMUTER_FASTEST       0x00 // Fastest (default)
#define SI470X_SMUTER_FAST          0x40 // Fast
#define SI470X_SMUTER_SLOW          0x80 // Slow
#define SI470X_SMUTER_SLOWEST       0xC0 // Slowest
#define SI470X_SMUTEA               0x30 // Softmute attenuation
#define SI470X_SMUTEA_16            0x00 // 16 dB
#define SI470X_SMUTEA_14            0x10 // 14 dB
#define SI470X_SMUTEA_12            0x20 // 12 dB
#define SI470X_SMUTEA_10            0x30 // 10 dB
#define SI470X_VOLEXT               0x01 // Extended volume range

// 9 byte (06L)
#define SI470X_SKSNR                0xF0 // Seek SNR threshold (default 0 = disabled)
#define SI470X_SKCNT                0x0F // Seek FM impulse detection threshold

// 10 byte (07H)
#define SI470X_XOSCEN               0x80 // Crystal oscillator enable (1)
#define SI470X_AHIZEN               0x40 // Audio High-Z enable (1)

// Read mode

// 0 byte (0AH)
#define SI740X_RDSR                 0x80 // RDS ready (1)
#define SI740X_STC                  0x40 // Seek/tune complete (1)
#define SI740X_SFBL                 0x20 // Seek failure or band limit reached (1)
#define SI740X_AFCRL                0x10 // AFC Railed (1)
#define SI740X_RDSS                 0x08 // RDS synchronized (1)
#define SI740X_BLERA                0x60 // 2 bits error level in block A
#define SI740X_ST                   0x01 // Stereo indicator (1)

// 1 byte (0AL)
#define SI740X_RSSI                 0xFF // RSSI signal level

// 2 byte (0BH)
#define SI740X_BLERB                0xC0 // 2 bits error level in block B
#define SI740X_BLERC                0x30 // 2 bits error level in block C
#define SI740X_BLERD                0x0C // 2 bits error level in block D
#define SI740X_READCHAN_9_8         0x03 // READCHAN 9..8 bits

// 3 byte (0BL)
#define SI740X_READCHAN_7_0         0xFF // READCHAN 7..0 bits

// 4 byte (0CH)
#define SI740X_RDSA_HI              0xFF // RDS block A data high
// 5 byte (0CL)
#define SI740X_RDSA_LO              0xFF // RDS block A data low

// 6 byte (0DH)
#define SI740X_RDSB_HI              0xFF // RDS block B data high
// 7 byte (0DL)
#define SI740X_RDSB_LO              0xFF // RDS block B data low

// 8 byte (0EH)
#define SI740X_RDSC_HI              0xFF // RDS block C data high
// 9 byte (0EL)
#define SI740X_RDSC_LO              0xFF // RDS block C data low

// 10 byte (0FH)
#define SI740X_RDSD_HI              0xFF // RDS block D data high
// 11 byte (0FL)
#define SI740X_RDSD_LO              0xFF // RDS block D data low

#ifdef __cplusplus
}
#endif

#endif // SI470X_REGS_H
