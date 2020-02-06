#include "usbd_amp.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"

static uint8_t USBD_AMP_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_AMP_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_AMP_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t *USBD_AMP_GetCfgDesc (uint16_t *length);
static uint8_t *USBD_AMP_GetDeviceQualifierDesc (uint16_t *length);
static uint8_t USBD_AMP_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

USBD_ClassTypeDef USBD_AMP = {
    USBD_AMP_Init,
    USBD_AMP_DeInit,
    USBD_AMP_Setup,
    NULL,                           // EP0_TxSent
    NULL,                           // EP0_RxReady
    USBD_AMP_DataIn,                // DataIn
    NULL,                           // DataOut
    NULL,                           // SOF
    NULL,
    NULL,
    USBD_AMP_GetCfgDesc,
    USBD_AMP_GetCfgDesc,
    USBD_AMP_GetCfgDesc,
    USBD_AMP_GetDeviceQualifierDesc,
};

// USB AMP device Configuration Descriptor
static uint8_t USBD_AMP_CfgDesc[USB_AMP_CONFIG_DESC_SIZ] __ALIGN_END = {
    // 0
    0x09,                           // bLength: Configuration Descriptor size
    USB_DESC_TYPE_CONFIGURATION,    // bDescriptorType: Configuration
    USB_AMP_CONFIG_DESC_SIZ,        // wTotalLength: Bytes returned
    0x00,
    0x01,                           // bNumInterfaces: 1 interface
    0x01,                           // bConfigurationValue: Configuration value
    0x00,                           // iConfiguration: Index of string descriptor describing the configuration
    0xC0,                           // bmAttributes: bus powered
    0x32,                           // MaxPower 100 mA: this current is used for detecting Vbus
    // 9
    0x09,                           // bLength: Interface Descriptor size
    USB_DESC_TYPE_INTERFACE,        // bDescriptorType: Interface descriptor type
    0x00,                           // bInterfaceNumber: Number of Interface
    0x00,                           // bAlternateSetting: Alternate setting
    0x01,                           // bNumEndpoints
    0x03,                           // bInterfaceClass: HID
    0x00,                           // bInterfaceSubClass : 1=BOOT, 0=no boot
    0x00,                           // nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
    0x00,                           // iInterface: Index of string descriptor
    // 18 = USB_AMP_DESC_OFT
    0x09,                           // bLength: AMP Descriptor size
    AMP_DESCRIPTOR_TYPE,            // bDescriptorType: AMP
    0x11,                           // bAMP_HID: HID Class Spec release number (1.11)
    0x01,
    0x00,                           // bCountryCode: Hardware target country
    0x01,                           // bNumDescriptors: Number of HID class descriptors to follow
    AMP_REPORT_DESC,                // bDescriptorType: Report
    USBD_AMP_REPORT_DESC_SIZE,      // wItemLength: Total length of Report descriptor
    0x00,
    // 27
    0x07,                           // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint
    AMP_EPIN_ADDR,                  // bEndpointAddress: Endpoint Address (IN)
    0x03,                           // bmAttributes: Interrupt endpoint
    AMP_EPIN_SIZE,                  // wMaxPacketSize: 4 Byte max
    0x00,
    0x0A,                           // bInterval: Polling Interval (10 ms)
    // 34
} ;

// USB Standard Device Descriptor
static uint8_t USBD_AMP_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
    USB_LEN_DEV_QUALIFIER_DESC,
    USB_DESC_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
};

static uint8_t USBD_AMP_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    (void)cfgidx;

    uint8_t ret = 0;
    USBD_AMP_HandleTypeDef     *hhid;

    // Open EP IN
    USBD_LL_OpenEP(pdev,
                   AMP_EPIN_ADDR,
                   USBD_EP_TYPE_INTR,
                   AMP_EPIN_SIZE);

    pdev->pClassData = USBD_malloc(sizeof (USBD_AMP_HandleTypeDef));

    if (pdev->pClassData == NULL) {
        ret = 1;
    } else {
        hhid = (USBD_AMP_HandleTypeDef *) pdev->pClassData;

        hhid->state = AMP_IDLE;
        ((USBD_AMP_ItfTypeDef *)pdev->pUserData)->Init();
    }

    return ret;
}

static uint8_t USBD_AMP_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    (void)cfgidx;

    // Close AMP EP IN
    USBD_LL_CloseEP(pdev, AMP_EPIN_ADDR);

    // FRee allocated memory
    if (pdev->pClassData != NULL) {
        ((USBD_AMP_ItfTypeDef *)pdev->pUserData)->DeInit();
        USBD_free(pdev->pClassData);
        pdev->pClassData = NULL;
    }

    return USBD_OK;
}

static uint8_t USBD_AMP_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    uint16_t len = 0;
    uint8_t  *pbuf = NULL;
    USBD_AMP_HandleTypeDef *hhid = (USBD_AMP_HandleTypeDef *)pdev->pClassData;

    switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_CLASS :
        switch (req->bRequest) {


        case AMP_REQ_SET_PROTOCOL:
            hhid->Protocol = (uint8_t)(req->wValue);
            break;

        case AMP_REQ_GET_PROTOCOL:
            USBD_CtlSendData (pdev,
                              (uint8_t *)&hhid->Protocol,
                              1);
            break;

        case AMP_REQ_SET_IDLE:
            hhid->IdleState = (uint8_t)(req->wValue >> 8);
            break;

        case AMP_REQ_GET_IDLE:
            USBD_CtlSendData (pdev,
                              (uint8_t *)&hhid->IdleState,
                              1);
            break;

        case AMP_REQ_SET_REPORT:
            hhid->IsReportAvailable = 1;
            USBD_CtlPrepareRx (pdev, hhid->Report_buf, (uint8_t)(req->wLength));

            break;
        default:
            USBD_CtlError (pdev, req);
            return USBD_FAIL;
        }
        break;

    case USB_REQ_TYPE_STANDARD:
        switch (req->bRequest) {
        case USB_REQ_GET_DESCRIPTOR:
            if ( req->wValue >> 8 == AMP_REPORT_DESC) {
                len = MIN(USBD_AMP_REPORT_DESC_SIZE, req->wLength);
                pbuf =  ((USBD_AMP_ItfTypeDef *)pdev->pUserData)->pReport;
            } else if ( req->wValue >> 8 == AMP_DESCRIPTOR_TYPE) {
                pbuf = USBD_AMP_CfgDesc + USB_AMP_DESC_OFT;
                len = MIN(USB_AMP_DESC_SIZ, req->wLength);
            }

            USBD_CtlSendData (pdev,
                              pbuf,
                              len);

            break;

        case USB_REQ_GET_INTERFACE :
            USBD_CtlSendData (pdev,
                              (uint8_t *)&hhid->AltSetting,
                              1);
            break;

        case USB_REQ_SET_INTERFACE :
            hhid->AltSetting = (uint8_t)(req->wValue);
            break;
        }
    }
    return USBD_OK;
}

uint8_t USBD_AMP_SendReport(USBD_HandleTypeDef  *pdev, uint8_t *report, uint16_t len)
{
    USBD_AMP_HandleTypeDef *hhid = (USBD_AMP_HandleTypeDef *)pdev->pClassData;

    if (pdev->dev_state == USBD_STATE_CONFIGURED ) {
        if (hhid->state == AMP_IDLE) {
            hhid->state = AMP_BUSY;
            USBD_LL_Transmit (pdev,
                              AMP_EPIN_ADDR,
                              report,
                              len);
        } else {
            return USBD_BUSY;
        }
    } else {
        return USBD_FAIL;
    }

    return USBD_OK;
}

static uint8_t  *USBD_AMP_GetCfgDesc (uint16_t *length)
{
    *length = sizeof (USBD_AMP_CfgDesc);
    return USBD_AMP_CfgDesc;
}

static uint8_t  USBD_AMP_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    (void)epnum;

    // Ensure that the FIFO is empty before a new transfer, this condition could
    // be caused by  a new transfer before the end of the previous transfer
    ((USBD_AMP_HandleTypeDef *)pdev->pClassData)->state = AMP_IDLE;

    return USBD_OK;
}

static uint8_t  *USBD_AMP_GetDeviceQualifierDesc (uint16_t *length)
{
    *length = sizeof (USBD_AMP_DeviceQualifierDesc);
    return USBD_AMP_DeviceQualifierDesc;
}

uint8_t  USBD_AMP_RegisterInterface(USBD_HandleTypeDef *pdev, USBD_AMP_ItfTypeDef *fops)
{
    uint8_t  ret = USBD_FAIL;

    if (fops != NULL) {
        pdev->pUserData = fops;
        ret = USBD_OK;
    }

    return ret;
}
