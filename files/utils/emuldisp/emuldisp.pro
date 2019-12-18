TARGET = emuldisp

QT += core gui
QT += widgets

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CFLAGS += -std=c11
INCLUDEPATH += ../../../display

CONFIG += _DISP_320x240

_DISP_160x128 {
    DEFINES += _DISP_160x128
    SOURCES +=  ../../../gui/lt160x128.c
}

_DISP_176x132 {
    DEFINES += _DISP_176x132
    SOURCES +=  ../../../gui/lt176x132.c
}

_DISP_220x176 {
    DEFINES += _DISP_220x176
    SOURCES +=  ../../../gui/lt220x176.c
}

_DISP_320x240 {
    DEFINES += _DISP_320x240
    SOURCES +=  ../../../gui/lt320x240.c
}

_DISP_400x240 {
    DEFINES += _DISP_400x240
    SOURCES +=  ../../../gui/lt400x240.c
}

_DISP_480x320 {
    DEFINES += _DISP_480x320
    SOURCES +=  ../../../gui/lt480x320.c
}

SOURCES += \
    ../../../actions.c \
    ../../../amp.c \
    ../../../audio/audio.c \
	../../../control.c \
    ../../../display/fonts/font-terminus-12.c \
    ../../../display/fonts/font-terminus-14.c \
    ../../../display/fonts/font-terminus-14b.c \
    ../../../display/fonts/font-terminus-16.c \
    ../../../display/fonts/font-terminus-16b.c \
    ../../../display/fonts/font-terminus-18.c \
    ../../../display/fonts/font-terminus-18b.c \
    ../../../display/fonts/font-terminus-20.c \
    ../../../display/fonts/font-terminus-20b.c \
    ../../../display/fonts/font-terminus-22.c \
    ../../../display/fonts/font-terminus-22b.c \
    ../../../display/fonts/font-terminus-24.c \
    ../../../display/fonts/font-terminus-24b.c \
    ../../../display/fonts/font-terminus-28.c \
    ../../../display/fonts/font-terminus-28b.c \
    ../../../display/fonts/font-terminus-32.c \
    ../../../display/fonts/font-terminus-32b.c \
    ../../../display/glcd.c \
    ../../../gui/canvas.c \
	../../../gui/fonts/font-terminus-dig-30.c \
	../../../gui/fonts/font-terminus-dig-40.c \
	../../../gui/fonts/font-terminus-dig-58.c \
	../../../gui/fonts/font-terminus-dig-64.c \
	../../../gui/fonts/font-terminus-dig-80.c \
	../../../gui/fonts/font-terminus-dig-96.c \
	../../../gui/fonts/font-terminus-dig-120.c \
	../../../gui/fonts/font-terminus-mod-18x3.c \
	../../../gui/fonts/font-terminus-mod-24x3.c \
	../../../gui/icons/icons-amp-24.c \
    ../../../gui/icons/icons-amp-32.c \
    ../../../gui/icons/icons-amp-48.c \
    ../../../gui/icons/icons-amp-64.c \
    ../../../gui/palette.c \
    ../../../gui/widget/spectrumcolumn.c \
    ../../../gui/widget/stripedbar.c \
    ../../../gui/widget/textedit.c \
    ../../../input.c \
    ../../../menu.c \
	../../../ringbuf.c \
    ../../../screen.c \
    ../../../settings.c \
    ../../../swtimers.c \
    ../../../tr/labels.c \
    ../../../tr/labels_by.c \
    ../../../tr/labels_fr.c \
    ../../../tr/labels_ro.c \
    ../../../tr/labels_ru.c \
    ../../../tr/labels_tr.c \
    ../../../tr/labels_ua.c \
    ../../../tuner/stations.c \
    ../../../tuner/tuner.c \
    ../../../utils.c \
    ampui.cpp \
    emuldisp.cpp \
    emuldisp_stub.c \
    main.cpp \
    rtc.c \


HEADERS += \
    ../../../actions.h \
    ../../../amp.h \
    ../../../audio/audio.h \
    ../../../audio/audiodefs.h \
	../../../control.h \
	../../../debug.h \
    ../../../display/fonts.h \
    ../../../display/dispdefs.h \
    ../../../display/dispdrv.h \
    ../../../display/glcd.h \
    ../../../eemul.h \
    ../../../gui/canvas.h \
    ../../../gui/icons.h \
    ../../../gui/layout.h \
    ../../../gui/palette.h \
    ../../../gui/widget/spectrumcolumn.h \
    ../../../gui/widget/stripedbar.h \
    ../../../gui/widget/textedit.h \
    ../../../karadio.h \
    ../../../menu.h \
    ../../../pins.h \
    ../../../rc.h \
	../../../ringbuf.h \
    ../../../rtc.h \
    ../../../screen.h \
    ../../../settings.h \
    ../../../spectrum.h \
    ../../../swtimers.h \
    ../../../tr/labels.h \
    ../../../tuner/rds.h \
    ../../../tuner/stations.h \
    ../../../tuner/tuner.h \
    ../../../tuner/tunerdefs.h \
    ../../../input.h \
    ../../../usb/usbhid.h \
    ../../../usb/hidkeys.h \
    ../../../utils.h \
    ampui.h \
    emuldisp.h \
    emuldisp_stub.h \


FORMS += \
    amp.ui
