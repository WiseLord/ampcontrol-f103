TARGET = emuldisp

QT += core gui
QT += widgets
TEMPLATE = app
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += EMUL_DISP

SOURCES += \
    emuldisp.cpp \
	emulscreen.c \
	main.cpp \
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
    ../../../canvas/canvas.c \
    ../../../canvas/gc160x128.c \
    ../../../canvas/gc176x132.c \
    ../../../canvas/gc220x176.c \
    ../../../canvas/gc320x240.c \
    ../../../canvas/gc400x240.c \
    ../../../canvas/gc480x320.c \
    ../../../canvas/gm128x64.c \
    ../../../menu.c \
    ../../../tuner/tuner.c \
    ../../../audio/audio.c \
    ../../../tr/labels_by.c \
    ../../../tr/labels_ru.c \
    ../../../tr/labels.c \


HEADERS += \
    emuldisp.h \
	emulscreen.h \
    ../../../display/fonts/fonts.h \
	../../../display/icons/icons.h \
	../../../display/glcd.h \r \
    ../../../display/dispdrv.h \
    ../../../canvas/canvas.h \
    ../../../menu.h \
    ../../../tuner/tuner.h \
    ../../../audio/audio.h \
    ../../../audio/audiodefs.h \
    ../../../tuner/tunerdefs.h \
    ../../../tr/labels.h \
    ../../../rtc.h \
	../../../rc.h \
