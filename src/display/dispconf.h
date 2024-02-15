#ifndef DISPCONF_H
#define DISPCONF_H

#ifdef __cplusplus
extern "C" {
#endif

// Third-party SPI library implementation
#ifdef _DISP_SPI
#include "spi.h"
#define SPI_DISPLAY             SPI2
#define DISP_WAIT_BUSY()        spiWaitBusy(SPI_DISPLAY)
#define DISP_SPI_INIT()         spiInit(SPI_DISPLAY, false)
#define DISP_SPI_SEND_BYTE(x)   spiSendByte(SPI_DISPLAY, x)
#else
#define DISP_WAIT_BUSY()        (void)0
#endif

// TFT LCD pins description
#ifdef _DISP_16BIT
#define DISP_DATA_HI_Port       B
#define DISP_DATA_HI_Pin        0xFF00U
#define DISP_DATA_LO_Port       C
#define DISP_DATA_LO_Pin        0x00FFU
#else
#define DISP_DATA_Port          B
#define DISP_DATA_Pin           0x00FFU
#endif

#define DISP_CS_Port            B
#define DISP_CS_Pin             12
#define DISP_RS_Port            B
#define DISP_RS_Pin             14
#define DISP_WR_Port            B
#define DISP_WR_Pin             15

#ifdef _DISP_READ_ENABLED
#define DISP_RD_Port            B
#define DISP_RD_Pin             11
#endif

#ifdef _DISP_RST_ENABLED
#define DISP_RST_Port           B
#define DISP_RST_Pin            10
#endif

#ifdef _DISP_BCKL_ENABLED
#define DISP_BCKL_Port          C
#define DISP_BCKL_Pin           13
#endif

#ifdef __cplusplus
}
#endif

#endif // DISPCONF_H
