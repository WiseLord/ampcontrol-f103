#ifndef USB_HID_H
#define USB_HID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "hidkeys.h"

#define KBD_KEYS     2

typedef struct {
    uint8_t id;
    uint8_t modifiers;
    uint8_t key[KBD_KEYS];
} HidKeyboard;

void usbHidInit(void);

void usbHidSendKey(HidKey key);

#ifdef __cplusplus
}
#endif

#endif // USB_HID_H
