# tests/tests.pro

QT += core testlib
QT -= gui

TARGET = filemonitor_tests
TEMPLATE = app
CONFIG += console testcase
CONFIG -= app_bundle

SOURCES += test_suite.cpp \
        ../src/ConsoleLog.cpp \
        ../src/FileContainer.cpp \
        ../src/FileMonitor.cpp

HEADERS += \
        ../include/ConsoleLog.h \
        ../include/FileContainer.h \
        ../include/FileMonitor.h \
        ../include/IFileContainer.h \
        ../include/ILog.h

INCLUDEPATH += ../include/
