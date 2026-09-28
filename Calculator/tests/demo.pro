QT += widgets testlib
CONFIG += c++17
TEMPLATE = app
TARGET = CalculatorDemo
INCLUDEPATH += ..
SOURCES += demo.cpp ../calculatorengine.cpp ../calculatorwindow.cpp
HEADERS += ../calculatorengine.h ../calculatorwindow.h
FORMS += ../calculatorwindow.ui
