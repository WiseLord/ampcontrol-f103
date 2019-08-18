#ifndef CONTROL_H
#define CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

void controlInit(void);

void controlIRQ(void);

void controlGetData(void);

#ifdef __cplusplus
}
#endif

#endif // CONTROL_H
