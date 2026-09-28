QT += widgets testlib
CONFIG += c++17 testcase
TEMPLATE = app
TARGET = CalculatorTests
INCLUDEPATH += ..
SOURCES += tests.cpp ../calculatorengine.cpp ../calculatorwindow.cpp
HEADERS += ../calculatorengine.h ../calculatorwindow.h
FORMS += ../calculatorwindow.ui
