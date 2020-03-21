#include "control.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "audio/audio.h"
#include "bt.h"
#include "debug.h"
#include "hwlibs.h"
#include "ringbuf.h"
#include "tr/labels.h"
#include "tuner/tuner.h"
#include "usart.h"
#include "utils.h"

#define CMDBUF_SIZE     256

#define REPORT_ENABLED

static RingBuf rbuf;
static char rbData[CMDBUF_SIZE];
static LineParse lp;

#define GENERATE_AUDIO_TUNE_TEXT(TUNE)  # TUNE,

static const char *const CTRL_AUDIO_TUNE[] = {
    FOREACH_AUDIO_TUNE(GENERATE_AUDIO_TUNE_TEXT)
};

static bool isEndOfLine(const char *line)
{
    return (*line < ' ');
}

static void controlParseAmpStatus(char *line)
{
    if (isEndOfLine(line)) {
        controlReportAmpStatus();
    }
}

static bool controlParseNumber(char **line, int16_t *number)
{
    char *end;
    int16_t value = (int16_t)strtol(*line, &end, 10);
    if (*line != end) {
        *number = value;
        *line = end;
        return true;
    }

    return false;
}

static void controlParseAmpAudio(char *line)
{
    if (isEndOfLine(line)) {
        controlReportAudioIc();
    }
}

static void controlParseAmpInput(char *line)
{
    // Follows "amp.input"
    if (isEndOfLine(line)) {
        controlReportAudioInput();
    } else if (utilIsPrefix(line, ".prev")) {
        ampActionQueue(ACTION_AUDIO_INPUT_CHANGE, -1);
    } else if (utilIsPrefix(line, ".next")) {
        ampActionQueue(ACTION_AUDIO_INPUT_CHANGE, +1);
    } else if (' ' == *line) {
        int16_t val;
        if (controlParseNumber(&line, &val)) {
            ampActionQueue(ACTION_AUDIO_INPUT_SET, val);
        }
    }
}

static void controlParseAmpTune(char *line, AudioTune tune)
{
    for (AudioTune t = AUDIO_TUNE_VOLUME; t < AUDIO_TUNE_END - 1; t++) {
        if (t == tune) {
            ampSelectTune(tune);
            if (isEndOfLine(line)) {
                controlReportAudioTune(tune);
            } else if (utilIsPrefix(line, ".dec")) {
                ampActionQueue(ACTION_AUDIO_PARAM_CHANGE, -1);
            } else if (utilIsPrefix(line, ".inc")) {
                ampActionQueue(ACTION_AUDIO_PARAM_CHANGE, +1);
            } else if (' ' == *line) {
                int16_t val;
                if (controlParseNumber(&line, &val)) {
                    ampActionQueue(ACTION_AUDIO_PARAM_SET, val);
                }
            }
        }
    }

    if (utilIsPrefix(line, ".volume")) {

    }
}

static void controlParseAmpTunerFreq(char *line)
{
    if (isEndOfLine(line)) {
        controlReportTunerFreq(true);
    } else if (' ' == *line) {
        int16_t val;
        if (controlParseNumber(&line, &val)) {
            ampActionQueue(ACTION_TUNER_SET_FREQ, val);
        }
    }
}

static void controlParseAmpTuner(char *line)
{
    // Follows "amp.tuner"
    if (isEndOfLine(line)) {
        controlReportTunerIc();
    } else if (utilIsPrefix(line, ".freq")) {
        controlParseAmpTunerFreq(line + strlen(".freq"));
    }
}


static void controlParseAmp(char *line)
{
    // Follows "amp"
    AmpStatus ampStatus = ampGet()->status;

    if (ampStatus != AMP_STATUS_ACTIVE) {
        if (utilIsPrefix(line, ".switch") || utilIsPrefix(line, ".on")) {
            ampActionQueue(ACTION_STANDBY, FLAG_EXIT);
        } else {
            controlReportAmpStatus();
            return;
        }
    }

    if (isEndOfLine(line)) {
        //
    } else if (utilIsPrefix(line, ".switch")) {
        ampActionQueue(ACTION_STANDBY, FLAG_SWITCH);
    } else if (utilIsPrefix(line, ".on")) {
        ampActionQueue(ACTION_STANDBY, FLAG_EXIT);
    } else if (utilIsPrefix(line, ".off")) {
        ampActionQueue(ACTION_STANDBY, FLAG_ENTER);
    } else if (utilIsPrefix(line, ".status")) {
        controlParseAmpStatus(line + strlen(".status"));

    } else if (utilIsPrefix(line, ".audio")) {
        controlParseAmpAudio(line + strlen(".audio"));
    } else if (utilIsPrefix(line, ".input")) {
        controlParseAmpInput(line + strlen(".input"));
    } else if (utilIsPrefix(line, ".volume")) {
        controlParseAmpTune(line + strlen(".volume"), AUDIO_TUNE_VOLUME);
    } else if (utilIsPrefix(line, ".bass")) {
        controlParseAmpTune(line + strlen(".bass"), AUDIO_TUNE_BASS);
    } else if (utilIsPrefix(line, ".middle")) {
        controlParseAmpTune(line + strlen(".middle"), AUDIO_TUNE_MIDDLE);
    } else if (utilIsPrefix(line, ".treble")) {
        controlParseAmpTune(line + strlen(".treble"), AUDIO_TUNE_TREBLE);

    } else if (utilIsPrefix(line, ".tuner")) {
        controlParseAmpTuner(line + strlen(".tuner"));
    }
}

static void controlParseLine(LineParse *lp)
{
    char *line = lp->line;

    if (utilIsPrefix(line, "amp")) {
        // Amp control
        controlParseAmp(line + strlen("amp"));
    } else {
        // BT201 control
        if (utilIsPrefix(line, "QM+")) {
            bt201ParseInput(line + strlen("QM+"));
        } else if (utilIsPrefix(line, "MU+")) {
            bt201ParseMount(line + strlen("MU+"));
        } else if (utilIsPrefix(line, "MF+")) {
            bt201ParseSongName(line + strlen("MF+"), lp->size - (int16_t)strlen("MF+"));
        }
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
    ringBufInit(&rbuf, rbData, sizeof(rbData));
}

void USART_DBG_HANDLER(void)
{
    // Check RXNE flag value in SR register
    if (LL_USART_IsActiveFlag_RXNE(USART_DBG) && LL_USART_IsEnabledIT_RXNE(USART_DBG)) {
        char ch = LL_USART_ReceiveData8(USART_DBG);
        ringBufPushChar(&rbuf, ch);
    } else {
        // Call Error function
    }
}

void controlGetData(void)
{
    while (ringBufGetSize(&rbuf) > 0) {
        char ch = ringBufPopChar(&rbuf);
        if (utilReadChar(&lp, ch)) {
            if (lp.line[0] != '\0') {
                controlParseLine(&lp);
            }
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

void controlReportAudioIc(void)
{
    AudioParam *par = &audioGet()->par;

    sendReport(utilMkStr("##AMP.AUDIO.IC#: %s", labelsGetDefault(LABEL_AUDIO_IC + par->ic)));
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

void controlReportAudioTunes(void)
{
    for (AudioTune tune = AUDIO_TUNE_VOLUME; tune < AUDIO_TUNE_END - 1; tune++) {
        controlReportAudioTune(tune);
    }
}

void controlReportTunerIc(void)
{
    TunerParam *par = &tunerGet()->par;

    sendReport(utilMkStr("##AMP.TUNER.IC#: %s", labelsGetDefault(LABEL_TUNER_IC + par->ic)));
}

void controlReportTunerFreq(bool force)
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
    controlReportAmpStatus();

    controlReportAudioIc();
    controlReportAudioInput();
    controlReportAudioTunes();

    controlReportTunerIc();
    controlReportTunerFreq(true);
}
