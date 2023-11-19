#include "relay.h"

#include "audio.h"
#include "pins.h"
#include "swtimers.h"

#define RELAY_IN_CNT    4

static const AudioGrid gridVolume    = {NULL, -1,  1, (int8_t)(2.00)}; // -1..1dB with 2dB step

static AudioParam *aPar;

static const AudioApi relayApi = {
    .init = relayInit,
    .getInCnt = relayGetInCnt,

    .setTune = relaySetTune,
    .setInput = relaySetInput,

    .setLoudness = relaySetLoudness,
};

const AudioApi *relayGetApi()
{
    return &relayApi;
}

void relayInit(AudioParam *param)
{
    aPar = param;
    aPar->grid[AUDIO_TUNE_VOLUME]      = &gridVolume;

    OUT(VOL_UP);
    OUT(VOL_DN);
    OUT(LOUDNESS);

    OUT(IN_0);
    OUT(IN_1);
    OUT(IN_2);
    OUT(IN_3);

    CLR(VOL_UP);
    CLR(VOL_DN);
    CLR(LOUDNESS);

    CLR(IN_0);
    CLR(IN_1);
    CLR(IN_2);
    CLR(IN_3);
}

int8_t relayGetInCnt(void)
{
    return RELAY_IN_CNT;
}

void relaySetTune(AudioTune tune, int8_t value)
{
    if (tune == AUDIO_TUNE_VOLUME) {
        if (value > 0) {
            swTimSet(SW_TIM_VOLUME_KEY, 200);
            SET(VOL_UP);
            CLR(VOL_DN);
        } else if (value < 0) {
            swTimSet(SW_TIM_VOLUME_KEY, 200);
            SET(VOL_DN);
            CLR(VOL_UP);
        }
    }
}

void relayReleaseVolume()
{
    if (swTimGet(SW_TIM_VOLUME_KEY) == 0) {
        CLR(VOL_UP);
        CLR(VOL_DN);
        swTimSet(SW_TIM_VOLUME_KEY, SW_TIM_OFF);
    }
}

static void clearInputs()
{
    CLR(IN_0);
    CLR(IN_1);
    CLR(IN_2);
    CLR(IN_3);
}

void relaySetInput(int8_t value)
{
    clearInputs();

    switch (value) {
    case 0:
        SET(IN_0);
        break;
    case 1:
        SET(IN_1);
        break;
    case 2:
        SET(IN_2);
        break;
    case 3:
        SET(IN_3);
        break;
    default:
        break;
    }
}

void relaySetLoudness(bool value)
{
    if (value) {
        SET(LOUDNESS);
    } else {
        CLR(LOUDNESS);
    }
}
