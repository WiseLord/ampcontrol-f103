#include "usbhid.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_amp.h"

#include <stm32f1xx_ll_utils.h>

#define REPORT_ID_KEYBOARD  0x01
#define REPORT_ID_MOUSE     0x02

static int8_t usbHidInitFS(void);
static int8_t usbHidDeInitFS(void);
static int8_t usbHidOutEventFS(uint8_t event_idx, uint8_t state);

static USBD_HandleTypeDef hUsbDeviceFS;

static HidKeyboard keyboardHID;

static uint8_t usbHidReportDescFS[USBD_AMP_REPORT_DESC_SIZE] __ALIGN_END = {
    // 41 bytes
    0x05, 0x01,                 //  Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,                 //  Usage (Keyboard)
    0xA1, 0x01,                 //  Collection (Application)
    0x85, REPORT_ID_KEYBOARD,   //      Report ID (1)
    0x05, 0x07,                 //      Usage Page (Kbrd/Keypad)
    0x75, 0x01,                 //      Report Size (1)
    0x95, 0x08,                 //      Report Count (8)
    0x19, 0xE0,                 //      Usage Minimum (0xE0)
    0x29, 0xE7,                 //      Usage Maximum (0xE7)
    0x15, 0x00,                 //      Logical Minimum (0)
    0x25, 0x01,                 //      Logical Maximum (1)
    0x81, 0x02,                 //      Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, KBD_KEYS,             //      Report Count (KBD_KEYS)
    0x75, 0x08,                 //      Report Size (8)
    0x15, 0x00,                 //      Logical Minimum (0)
    0x25, 0xff,                 //      Logical Maximum (255)
    0x05, 0x07,                 //      Usage Page (Kbrd/Keypad)
    0x19, 0x00,                 //      Usage Minimum (0)
    0x29, 0xff,                 //      Usage Maximum (255)
    0x81, 0x00,                 //      Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,                       //  End Collection
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
    do {
        status = USBD_AMP_SendReport(&hUsbDeviceFS, (uint8_t *)data, size);
        if (status == USBD_FAIL) {
            // ERROR
        }
    } while (status == USBD_BUSY);
}

void usbHidInit(void)
{
    keyboardHID.id = REPORT_ID_KEYBOARD;
    keyboardHID.modifiers = 0;

    for (uint8_t i = 0; i < KBD_KEYS; i++) {
        keyboardHID.key[i] = 0;
    }

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
