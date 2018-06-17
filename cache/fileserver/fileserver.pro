QT       += core websockets
QT       -= gui

TARGET = fileserver
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    fileserver.cpp \
    filereader.cpp \
    requesthandler.cpp \
    responsedispatcherthread.cpp \
    readerwritercache.cpp \
    hoaremonitor.cpp

HEADERS += \
    fileserver.h \
    filereader.h \
    response.h \
    abstractbuffer.h \
    request.h \
    requesthandler.h \
    responsedispatcherthread.h \
    requestdispatcherthread.h \
    buffer.h \
    requestworker.h \
    readerwritercache.h \
    readerwriterlock.h \
    option.h \
    hoaremonitor.h

EXAMPLE_FILES += fileclient.html

