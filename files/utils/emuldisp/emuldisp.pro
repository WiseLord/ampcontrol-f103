TARGET = emuldisp

QT += core gui
QT += widgets
TEMPLATE = app
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += EMUL_DISP

SOURCES += \
    emuldisp.cpp \
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
    ../../../display/fonts/font-terminus-mod-44.c \
    ../../../display/fonts/font-terminus-mod-64.c \
    ../../../display/fonts/font-terminus-mod-96.c \
    ../../../display/icons-24.c \
    ../../../display/icons-32.c \
    ../../../display/glcd.c

HEADERS += \
    emuldisp.h \
    ../../../display/fonts/fonts.h \
    ../../../display/icons.h \
    ../../../display/glcd.h \r \
    ../../../display/dispdrv.h
    ../../../display/dispdrv.h
