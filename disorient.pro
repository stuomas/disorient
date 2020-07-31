VERSION = 1.0.0

TARGET = disorient-$$VERSION
CONFIG += static
QT += core gui widgets websockets mqtt

RESOURCES += \
    resources.qrc

FORMS += \
    mainwindow.ui

DISTFILES += \
    disorient.pro.user

HEADERS += \
    constants.h \
    endpoint.h \
    inputmqtt.h \
    inputwebsocket.h \
    mainwindow.h \
    PolicyConfig.h \
    audioendpointcontroller.h

SOURCES += \
    endpoint.cpp \
    inputmqtt.cpp \
    inputwebsocket.cpp \
    main.cpp \
    mainwindow.cpp \
    audioendpointcontroller.cpp

RC_ICONS = icon.ico

win32: LIBS += -lOle32 -lUuid -lUser32
