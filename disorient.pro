VERSION = 0.1.2

include(disorient.pri)

TARGET = disorient-$$VERSION
CONFIG += static
QT += core gui widgets websockets mqtt

DISTFILES += \
    disorient.pri \
    disorient.pro.user

RC_ICONS = resources/icons/icon.ico

win32: LIBS += -lOle32 -lUuid -lUser32
