# tests/tests.pro

QT += core testlib
QT -= gui

TARGET = filemonitor_tests
TEMPLATE = app
CONFIG += console testcase
CONFIG -= app_bundle

# Исходники тестов
SOURCES += test_suite.cpp

SOURCES += \
        ../src/ConsoleLog.cpp \
        ../src/DynamicFileContainer.cpp \
        ../src/FileMonitor.cpp \

HEADERS += \
        ../include/ConsoleLog.h \
        ../include/DynamicFileContainer.h \
        ../include/FileMonitor.h \
        ../include/IFileContainer.h \
        ../include/ILog.h

INCLUDEPATH += ../include/
