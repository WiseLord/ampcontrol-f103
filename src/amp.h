#ifndef AMP_H
#define AMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "action.h"
#include "audio/audiodefs.h"
#include "gui/canvas.h"
#include "gui/icons.h"

#define FLAG_EXIT           0
#define FLAG_ENTER          1
#define FLAG_SWITCH         2

typedef struct {
    ActionType type;
    ScreenType screen;

    int16_t value;
    int16_t timeout;

    ScreenType prevScreen;
} Action;

typedef uint8_t AmpStatus;
enum {
    AMP_STATUS_STBY,
    AMP_STATUS_POWERED,
    AMP_STATUS_INIT,
    AMP_STATUS_HW_READY,
    AMP_STATUS_ACTIVE,

    AMP_STATUS_END
};

typedef uint8_t InputType;
enum {
    IN_TUNER = 0,
    IN_PC,
    IN_TV,
    IN_BLUETOOTH,
    IN_DVD,
    IN_USB,
    IN_MICROPHONE,
    IN_GUITAR,
    IN_TURNTABLES,
    IN_SDCARD,
    IN_PROJECTOR,
    IN_SATELLITE,
    IN_MIXER,
    IN_KARADIO,

    IN_END
};

typedef struct {
    AmpStatus status;
    ScreenType screen;
    ScreenType defScreen;
    ScreenType prevScreen;
    bool clearScreen;

    Icon iconHint;
    int8_t brightness;

    InputType inType[MAX_INPUTS];
    uint8_t inputStatus;
    int8_t volume;
    bool showDb;
} Amp;

typedef uint16_t MuteStby;
enum {
    MUTESTBY_POS = 0,
    MUTESTBY_NEG,

    MUTESTBY_END,
};

void ampPinMute(bool value);
void ampPinStby(bool value);
void ampInitMuteStby(void);

void ampSelectTune(AudioTune tune);

void ampInit(void);
void ampRun(void);

Amp *ampGet(void);

void ampActionQueue(ActionType type, int16_t value);

void ampSetBrightness(int8_t value);

#ifdef __cplusplus
}
#endif

#endif // AMP_H
