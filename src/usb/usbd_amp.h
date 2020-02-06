#ifndef AMP_H
#define AMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include  "usbd_ioreq.h"

#define AMP_EPIN_ADDR                 0x81
#define AMP_EPIN_SIZE                 0x04

#define USB_AMP_CONFIG_DESC_SIZ       34

#define USB_AMP_DESC_OFT              18
#define USB_AMP_DESC_SIZ              9

#define AMP_DESCRIPTOR_TYPE           0x21
#define AMP_REPORT_DESC               0x22

#define AMP_REQ_SET_PROTOCOL          0x0B
#define AMP_REQ_GET_PROTOCOL          0x03

#define AMP_REQ_SET_IDLE              0x0A
#define AMP_REQ_GET_IDLE              0x02

#define AMP_REQ_SET_REPORT            0x09
#define AMP_REQ_GET_REPORT            0x01

typedef enum {
    AMP_IDLE = 0,
    AMP_BUSY,
}
AMP_StateTypeDef;

typedef struct _USBD_AMP_Itf {
    uint8_t                  *pReport;
    int8_t (* Init)          (void);
    int8_t (* DeInit)        (void);
    int8_t (* OutEvent)      (uint8_t, uint8_t );

} USBD_AMP_ItfTypeDef;

typedef struct {
    uint8_t              Report_buf[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE];
    uint32_t             Protocol;
    uint32_t             IdleState;
    uint32_t             AltSetting;
    uint32_t             IsReportAvailable;
    AMP_StateTypeDef     state;
}
USBD_AMP_HandleTypeDef;

extern USBD_ClassTypeDef    USBD_AMP;
#define USBD_AMP_CLASS      &USBD_AMP

uint8_t USBD_AMP_SendReport (USBD_HandleTypeDef *pdev,
                             uint8_t *report,
                             uint16_t len);

uint8_t  USBD_AMP_RegisterInterface (USBD_HandleTypeDef   *pdev,
                                     USBD_AMP_ItfTypeDef *fops);

#ifdef __cplusplus
}
#endif

#endif // AMP_H
