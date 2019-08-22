#ifndef USBD_CONF_H
#define USBD_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stm32f1xx.h>
#include <stm32f1xx_hal_def.h>

#define USBD_MAX_NUM_INTERFACES             1
#define USBD_MAX_NUM_CONFIGURATION          1
#define USBD_MAX_STR_DESC_SIZ               64
#define USBD_SUPPORT_USER_STRING            0
#define USBD_SELF_POWERED                   1
#define USBD_CUSTOMHID_OUTREPORT_BUF_SIZE   2
#define USBD_AMP_REPORT_DESC_SIZE           78

#define DEVICE_FS           0

#define USBD_malloc         (uint32_t *)USBD_static_malloc
#define USBD_free           USBD_static_free

void *USBD_static_malloc(uint32_t size);
void USBD_static_free(void *p);

void USBD_IRQ(void);

#define USBD_UsrLog(...)
#define USBD_ErrLog(...)
#define USBD_DbgLog(...)

#ifdef __cplusplus
}
#endif

#endif // USBD_CONF_H
