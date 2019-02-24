#ifndef L2F50126_REGS_H
#define L2F50126_REGS_H

#define L2F50126_DATCTL     0xBC    // Data Control (data handling in RAM)
#define L2F50126_DISCTL     0xCA    // Display Control
#define L2F50126_GCP64      0xCB    // pulse set for 64 gray scale
#define L2F50126_GCP16      0xCC    // pulse set for 16 gray scale
#define L2F50126_OSSEL      0xD0    // Oscillator select
#define L2F50126_GSSET      0xCD    // set for gray scales
#define L2F50126_ASCSET     0xAA    // aerea scroll setting
#define L2F50126_SCSTART    0xAB    // scroll start setting
#define L2F50126_DISON      0xAF    // Display ON (no parameter)
#define L2F50126_DISOFF     0xAE    // Display OFF (no parameter)
#define L2F50126_DISINV     0xA7    // Display Invert (no parameter)
#define L2F50126_DISNOR     0xA6    // Display Normal (no parameter)
#define L2F50126_SLPIN      0x95    // Display Sleep (no parameter)
#define L2F50126_SLPOUT     0x94    // Display out of sleep (no parameter)
#define L2F50126_RAMWR      0x5C    // Display Memory write
#define L2F50126_PTLIN      0xA8    // partial screen write

#define SD_CSET 0x15                // column address setting
#define SD_PSET 0x75                // page address setting

#endif // L2F50126_REGS_H
