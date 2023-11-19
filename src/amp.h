#ifndef AMP_H
#define AMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "action.h"
#include "audio/audio.h"
#include "gui/canvas.h"
#include "gui/icons.h"

#define FLAG_EXIT           0
#define FLAG_ENTER          1
#define FLAG_SWITCH         2

#define LCD_BR_MIN          1
#define LCD_BR_MAX          32

#define ALLOW_SWD           (audioGet()->par.ic != AUDIO_IC_RELAY)

typedef uint8_t ScreenType;
enum {
    // Screens allowed to be default
    SCREEN_SPECTRUM = 0,
    SCREEN_TIME,
    SCREEN_AUDIO_INPUT,

    SCREEN_STANDBY,

    SCREEN_AUDIO_PARAM,
    SCREEN_AUDIO_FLAG,

    SCREEN_MENU,

    SCREEN_TEXTEDIT,

    SCREEN_STBY_TIMER,
    SCREEN_SILENCE_TIMER,

    SCREEN_SAVER,

    SCREEN_END
};

typedef uint8_t AmpStatus;
enum {
    AMP_STATUS_STBY,
    AMP_STATUS_POWERED,
    AMP_STATUS_INIT,
    AMP_STATUS_HW_READY,
    AMP_STATUS_ACTIVE,

    AMP_STATUS_END
};

typedef int8_t InputType;
enum {
    IN_DISABLED = -1,

    IN_TUNER = 0,
    IN_PC,
    IN_AUX,
    IN_BLUETOOTH,
    IN_KARADIO,
    IN_USB,
    IN_SDCARD,
    IN_MPD,

    IN_END,
};

typedef struct {
    AmpStatus status;
    ScreenType screen;
    ScreenType defScreen;

    InputType inType[MAX_INPUTS];

    bool showDb;
    bool stbyLow;
    bool muteLow;
    uint8_t silenceTimer;
    int8_t brStby;
    int8_t brWork;
    bool scrSaver;
} Amp;

typedef uint16_t MuteStby;
enum {
    MUTESTBY_POS = 0,
    MUTESTBY_NEG,

    MUTESTBY_END,
};

void ampInit(void);
Amp *ampGet(void);

void ampRun(void);

void ampSyncFromOthers(void);
void ampSyncToOthers(void);

void ampActionGet(void);
void ampActionRemap(void);
void ampActionHandle(void);
void ampScreenShow(void);

Action ampGetButtons();
Action ampGetEncoder(void);


void ampPinMute(bool value);
void ampPinStby(bool value);

void ampSelectTune(AudioTune tune);
void ampActionQueue(ActionType type, int16_t value);
void ampSetBrightness(int8_t value);

void ampUpdateDate(char *date);

#ifdef __cplusplus
}
#endif

#endif // AMP_H
