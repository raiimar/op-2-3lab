QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    context_state.cpp \
    iterator.cpp \
    list.cpp \
    load_data.cpp \
    main.cpp \
    mainwindow.cpp \
    metrics_calc.cpp \
    parser.cpp

HEADERS += \
    appcontext.h \
    context_state.h \
    data.h \
    iterator.h \
    list.h \
    load_data.h \
    mainwindow.h \
    metrics_calc.h \
    parser.h \
    status.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
