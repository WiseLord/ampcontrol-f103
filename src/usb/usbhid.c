#include "usbhid.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_amp.h"

#include "hwlibs.h"

#include <string.h>

#define REPORT_ID_KEYBOARD      0x01
#define REPORT_ID_MEDIAKEYS     0x02

static int8_t usbHidInitFS(void);
static int8_t usbHidDeInitFS(void);
static int8_t usbHidOutEventFS(uint8_t event_idx, uint8_t state);

static USBD_HandleTypeDef hUsbDeviceFS;

static HidKeyboard keyboardHID;
static HidMediaKeys mediakeysHID;

static uint8_t usbHidReportDescFS[USBD_AMP_REPORT_DESC_SIZE] __ALIGN_END = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x01,                    //   REPORT_ID (1)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x02,                    //   REPORT_COUNT (2)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x7f,                    //   USAGE_MAXIMUM (Keyboard Mute)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x7f,                    //   LOGICAL_MAXIMUM (127)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0,                          // END_COLLECTION
    0x05, 0x0c,                    // USAGE_PAGE (Consumer Devices)
    0x09, 0x01,                    // USAGE (Consumer Control)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x02,                    //   REPORT_ID (2)
    0x05, 0x0c,                    //   USAGE_PAGE (Consumer Devices)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x09, 0xb6,                    //   USAGE (Scan Previous Track)
    0x09, 0xb5,                    //   USAGE (Scan Next Track)
    0x09, 0xb7,                    //   USAGE (Stop)
    0x09, 0xb0,                    //   USAGE (Play)
    0x09, 0xb1,                    //   USAGE (Pause)
    0x09, 0xb4,                    //   USAGE (Rewind)
    0x09, 0xb3,                    //   USAGE (Fast Forward)
    0x09, 0xe2,                    //   USAGE (Mute)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

static USBD_AMP_ItfTypeDef USBD_AmpHidFopsFS = {
    usbHidReportDescFS,
    usbHidInitFS,
    usbHidDeInitFS,
    usbHidOutEventFS
};

static int8_t usbHidInitFS(void)
{
    return (USBD_OK);
}

static int8_t usbHidDeInitFS(void)
{
    return (USBD_OK);
}

static int8_t usbHidOutEventFS(uint8_t event_idx, uint8_t state)
{
    (void)event_idx;
    (void)state;

    return (USBD_OK);
}

static void usbHidSend(void *data, uint16_t size)
{
    uint8_t status;
    uint8_t timeoutMs = 25;
    do {
        status = USBD_AMP_SendReport(&hUsbDeviceFS, (uint8_t *)data, size);
        if (status == USBD_FAIL) {
            // ERROR
        }
        if (LL_SYSTICK_IsActiveCounterFlag()) {
            timeoutMs--;
        }
    } while (status == USBD_BUSY && timeoutMs != 0);
}

void usbHidInit(void)
{
    memset(&keyboardHID, 0, sizeof (keyboardHID));
    memset(&mediakeysHID, 0, sizeof (mediakeysHID));

    keyboardHID.id = REPORT_ID_KEYBOARD;
    mediakeysHID.id = REPORT_ID_MEDIAKEYS;

    USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
    USBD_RegisterClass(&hUsbDeviceFS, &USBD_AMP);
    USBD_AMP_RegisterInterface(&hUsbDeviceFS, &USBD_AmpHidFopsFS);
    USBD_Start(&hUsbDeviceFS);
}

void usbHidSendKey(HidKey key)
{
    keyboardHID.modifiers = 0;

    keyboardHID.key[0] = key;
    usbHidSend(&keyboardHID, sizeof(keyboardHID));

    keyboardHID.key[0] = 0;
    usbHidSend(&keyboardHID, sizeof(keyboardHID));
}

void usbHidSendMediaKey(HidMediaKey key)
{
    mediakeysHID.key = key;
    usbHidSend(&mediakeysHID, sizeof(mediakeysHID));
    mediakeysHID.key = HIDMEDIAKEY_NONE;
    usbHidSend(&mediakeysHID, sizeof(mediakeysHID));
}
