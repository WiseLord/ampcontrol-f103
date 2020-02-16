#include "bt.h"

#include "debug.h"

static void btPlay()
{
    dbg("AT+CB");
}

static void btPrevTrack()
{
    dbg("AT+CD");
}

static void btNextTrack()
{
    dbg("AT+CC");
}

void btSendMediaKey(HidMediaKey cmd)
{
    switch (cmd) {
    case HIDMEDIAKEY_PLAY:
        btPlay();
        break;
    case HIDMEDIAKEY_PREV_TRACK:
        btPrevTrack();
        break;
    case HIDMEDIAKEY_NEXT_TRACK:
        btNextTrack();
        break;
    default:
        break;
    }
}
