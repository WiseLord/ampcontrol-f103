#ifndef CONTROL_H
#define CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "actions.h"

void controlInit(void);

void controlIRQ(void);

void controlGetData(void);

void controlReportAmpStatus(void);
void controlReportInput(void);

#ifdef __cplusplus
}
#endif

#endif // CONTROL_H
