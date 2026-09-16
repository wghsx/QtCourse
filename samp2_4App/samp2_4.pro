#-------------------------------------------------
#
# Project created by QtCreator 2016-11-14T00:17:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg

TARGET = samp2_4
TEMPLATE = app


SOURCES += main.cpp\
        qwmainwind.cpp

HEADERS  += qwmainwind.h

FORMS    += qwmainwind.ui

RESOURCES += \
    res.qrc

RC_ICONS = AppIcon.ico

#-------------------------------------------------
# 显式指定 MinGW 工具链，避免与系统里其他 MinGW
# （如 VS Code 自带的 UCRT 版本）冲突导致链接失败
#-------------------------------------------------
MINGW_BIN = E:/Qt/Tools/mingw1310_64/bin
QMAKE_CC    = $$MINGW_BIN/gcc.exe
QMAKE_CXX   = $$MINGW_BIN/g++.exe
QMAKE_LINK  = $$MINGW_BIN/g++.exe
QMAKE_LINK_C = $$MINGW_BIN/gcc.exe
QMAKE_RC    = $$MINGW_BIN/windres.exe

