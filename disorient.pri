INCLUDEPATH += include src forms resources
DEPENDPATH += include src forms resources

HEADERS += \
    include/constants.h \
    include/endpoint.h \
    include/inputmqtt.h \
    include/inputwebsocket.h \
    include/mainwindow.h \
    include/PolicyConfig.h \
    include/audioendpointcontroller.h

SOURCES += \
    src/endpoint.cpp \
    src/inputmqtt.cpp \
    src/inputwebsocket.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/audioendpointcontroller.cpp

RESOURCES += \
    resources/resources.qrc

FORMS += \
    forms/mainwindow.ui
