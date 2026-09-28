QT += widgets
CONFIG += c++17
TEMPLATE = app
TARGET = Calculator

SOURCES += main.cpp \
           calculatorwindow.cpp \
           calculatorengine.cpp
HEADERS += calculatorwindow.h \
           calculatorengine.h
FORMS += calculatorwindow.ui
