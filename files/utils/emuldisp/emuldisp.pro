TARGET = emuldisp

QT += core gui
QT += widgets

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CFLAGS += -std=c11

DEFINES += EMUL_DISP

SOURCES += \
    amp.cpp \
    emuldisp.cpp \
    emuldisp_stub.c \
    main.cpp \
    rtc.c \
    ../../../audio/audio.c \
    ../../../canvas/canvas.c \
    ../../../canvas/layout.c \
    ../../../canvas/lt160x128.c \
    ../../../canvas/lt176x132.c \
    ../../../canvas/lt220x176.c \
    ../../../canvas/lt320x240.c \
    ../../../canvas/lt400x240.c \
    ../../../canvas/lt480x320.c \
    ../../../display/fonts/font-amp-08.c \
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
    ../../../display/fonts/font-terminus-dig-22.c \
    ../../../display/fonts/font-terminus-dig-30.c \
    ../../../display/fonts/font-terminus-dig-40.c \
    ../../../display/fonts/font-terminus-dig-42.c \
    ../../../display/fonts/font-terminus-dig-58.c \
    ../../../display/fonts/font-terminus-dig-64.c \
    ../../../display/fonts/font-terminus-dig-80.c \
    ../../../display/fonts/font-terminus-dig-96.c \
    ../../../display/fonts/font-terminus-dig-120.c \
    ../../../display/fonts/font-terminus-mod-18x3.c \
    ../../../display/fonts/font-terminus-mod-22x2.c \
    ../../../display/fonts/font-terminus-mod-24x2.c \
    ../../../display/fonts/font-terminus-mod-24x3.c \
    ../../../display/fonts/font-terminus-mod-32x2.c \
    ../../../display/fonts/font-terminus-mod-32x3.c \
    ../../../display/icons/icons-amp-24.c \
    ../../../display/icons/icons-amp-32.c \
    ../../../display/icons/icons-amp-48.c \
    ../../../display/icons/icons-amp-64.c \
    ../../../display/glcd.c \
    ../../../tr/labels.c \
    ../../../tr/labels_by.c \
    ../../../tr/labels_ru.c \
    ../../../tr/labels_tr.c \
    ../../../tr/labels_ua.c \
    ../../../tuner/stations.c \
    ../../../tuner/tuner.c \
    ../../../actions.c \
    ../../../eemap.c \
    ../../../input.c \
    ../../../menu.c \
    ../../../screen.c \
    ../../../swtimers.c \


HEADERS += \
    amp.h \
    emuldisp.h \
    ../../../audio/audio.h \
    ../../../audio/audiodefs.h \
    ../../../canvas/canvas.h \
    ../../../canvas/layout.h \
    ../../../display/fonts/fonts.h \
    ../../../display/icons/icons.h \
    ../../../display/dispdefs.h \
    ../../../display/dispdrv.h \
    ../../../display/glcd.h \
    ../../../tr/labels.h \
    ../../../tuner/stations.h \
    ../../../tuner/tuner.h \
    ../../../tuner/tunerdefs.h \
    ../../../actions.h \
    ../../../eemap.h \
    ../../../eemul.h \
    ../../../input.h \
    ../../../menu.h \
    ../../../rc.h \
    ../../../rtc.h \
    ../../../screen.h \
    ../../../spectrum.h \
    ../../../swtimers.h \
    ../../../usb/usbhid.h \
    ../../../usb/hidkeys.h \
    emuldisp_stub.h


FORMS += \
    amp.ui
