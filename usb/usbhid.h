#ifndef USB_HID_H
#define USB_HID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "hidkeys.h"

typedef struct {
    uint8_t id;
    uint8_t modifiers;
    uint8_t key[6];
} HidKeyboard;

void usbHidInit(void);

void usbHidSendKey(HidKey key);

#ifdef __cplusplus
}
#endif

#endif // USB_HID_H
