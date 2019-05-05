#ifndef EMULDISP_STUB_H
#define EMULDISP_STUB_H

#define EMUL_DISP_BORDER    5
#define EMUL_DISP_SCALE     2

#if defined (_DISP_160x128)
#define EMUL_DISP_WIDTH     160
#define EMUL_DISP_HEIGHT    128
#elif defined (_DISP_176x132)
#define EMUL_DISP_WIDTH     176
#define EMUL_DISP_HEIGHT    132
#elif defined (_DISP_220x176)
#define EMUL_DISP_WIDTH     220
#define EMUL_DISP_HEIGHT    176
#elif defined (_DISP_320x240)
#define EMUL_DISP_WIDTH     320
#define EMUL_DISP_HEIGHT    240
#elif defined (_DISP_400x240)
#define EMUL_DISP_WIDTH     400
#define EMUL_DISP_HEIGHT    240
#elif defined (_DISP_480x320)
#define EMUL_DISP_WIDTH     480
#define EMUL_DISP_HEIGHT    320
#else
#error "Unsupported display size"
#endif

#endif // EMULDISP_STUB_H
