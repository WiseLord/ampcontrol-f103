#include "control.h"

#include <stdbool.h>
#include <string.h>

#include "audio/audio.h"
#include "debug.h"
#include "hwlibs.h"
#include "ringbuf.h"
#include "tuner/tuner.h"
#include "usart.h"
#include "utils.h"

#define CMDBUF_SIZE     64

#define REPORT_ENABLED

static RingBuf cmdRb;
static char cmdRbData[CMDBUF_SIZE];
static LineParse cmdLp;

static const char *CTRL_AMP     = "amp";

static const char *CTRL_STATUS  = ".status";
static const char *CTRL_INPUT   = ".input";

static const char *CTRL_ON      = ".on";
static const char *CTRL_OFF     = ".off";
static const char *CTRL_TOGGLE  = ".togggle";

static const char *CTRL_PREV    = ".prev";
static const char *CTRL_NEXT    = ".next";

#define GENERATE_AUDIO_TUNE_TEXT(TUNE)  # TUNE,

static const char *const CTRL_AUDIO_TUNE[] = {
    FOREACH_AUDIO_TUNE(GENERATE_AUDIO_TUNE_TEXT)
};

static bool isEndOfLine(const char *line)
{
    if (*line == '\0') {
        return true;
    } else if (*line == '\r' || *line == '\n') {
        return true;
    }
    return false;
}

static void controlParseAmpStatus(char *line)
{
    if (isEndOfLine(line)) {
        controlReportAmpStatus();
    }
}

static void controlParseAmpInput(char *line)
{
    if (isEndOfLine(line)) {
        controlReportAudioInput();
    } else if (strstr(line, CTRL_PREV) == line) {
        ampActionQueue(ACTION_AUDIO_INPUT, -1);
    } else if (strstr(line, CTRL_NEXT) == line) {
        ampActionQueue(ACTION_AUDIO_INPUT, +1);
    }
}

static void controlParseAmp(char *line)
{
    if (isEndOfLine(line)) {
        //
    } else if (strstr(line, CTRL_ON) == line) {
        ampActionQueue(ACTION_STANDBY, FLAG_EXIT);
    } else if (strstr(line, CTRL_OFF) == line) {
        ampActionQueue(ACTION_STANDBY, FLAG_ENTER);
    } else if (strstr(line, CTRL_TOGGLE) == line) {
        ampActionQueue(ACTION_STANDBY, FLAG_SWITCH);
    } else if (strstr(line, CTRL_STATUS) == line) {
        controlParseAmpStatus(line + strlen(CTRL_STATUS));
    } else if (strstr(line, CTRL_INPUT) == line) {
        controlParseAmpInput(line + strlen(CTRL_INPUT));
    }
}

static void controlParseLine(char *line)
{
    if (strstr(line, CTRL_AMP) == line) {
        controlParseAmp(line + strlen(CTRL_AMP));
    }
}

static void sendReport(char *report)
{
#ifdef REPORT_ENABLED
    dbg(report);
#endif
}

void controlInit(void)
{
    ringBufInit(&cmdRb, cmdRbData, sizeof(cmdRbData));
}

void USART_DBG_HANDLER(void)
{
    // Check RXNE flag value in SR register
    if (LL_USART_IsActiveFlag_RXNE(USART_DBG) && LL_USART_IsEnabledIT_RXNE(USART_DBG)) {
        char data = LL_USART_ReceiveData8(USART_DBG);
#ifdef _DEBUG_KARADIO
        usartSendChar(USART_KARADIO, data);
#endif
        ringBufPushChar(&cmdRb, data);
    } else {
        // Call Error function
    }
}

void controlGetData(void)
{
    uint16_t size = ringBufGetSize(&cmdRb);

    for (uint16_t i = 0; i < size; i++) {
        char ch = ringBufPopChar(&cmdRb);
        if (utilReadChar(&cmdLp, ch)) {
            controlParseLine(cmdLp.line);
        }
    }
}

void controlReportAmpStatus(void)
{
    AmpStatus ampStatus = ampGet()->status;

    char *status = "UNKNOWN";

    switch (ampStatus) {
    case AMP_STATUS_STBY:
        status = "STANDBY";
        break;
    case AMP_STATUS_HW_READY:
    case AMP_STATUS_ACTIVE:
        status = "ACTIVE";
        break;
    }

    sendReport(utilMkStr("##AMP.STATUS#: %s", status));
}

void controlReportAudioInput(void)
{
    AudioParam *par = &audioGet()->par;

    sendReport(utilMkStr("##AMP.AUDIO.INPUT#: %u", par->input));
}

void controlReportAudioTune(AudioTune tune)
{
    AudioParam *par = &audioGet()->par;

    sendReport(utilMkStr("##AMP.AUDIO.%s#: %d", CTRL_AUDIO_TUNE[tune], par->tune[tune].value));
}

void controlReportAudio(void)
{
    controlReportAudioInput();
    for (AudioTune tune = AUDIO_TUNE_VOLUME; tune < AUDIO_TUNE_END - 1; tune++) {
        controlReportAudioTune(tune);
    }
}

void controlReportTuner(bool force)
{
    TunerStatus *st = &tunerGet()->status;

    static TunerStatus stOld;

    if (force || stOld.freq != st->freq) {
        sendReport(utilMkStr("##AMP.TUNER.FREQ#: %d", st->freq));
    }

    memcpy(&stOld, st, sizeof (stOld));
}

void controlReportAll(void)
{
    controlReportAudio();
}
