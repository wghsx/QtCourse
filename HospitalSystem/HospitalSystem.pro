QT += core gui widgets sql

CONFIG += c++17
TEMPLATE = app
TARGET = HospitalSystem

INCLUDEPATH += $$PWD

SOURCES += \
    main.cpp \
    model/databasemanager.cpp \
    model/historyrepository.cpp \
    model/patientrepository.cpp \
    model/userrepository.cpp \
    view/departmentpage.cpp \
    view/doctorpage.cpp \
    view/historydialog.cpp \
    view/loginpage.cpp \
    view/mainwindow.cpp \
    view/patientdialog.cpp \
    view/patientpage.cpp \
    view/registerdialog.cpp \
    view/sexdelegate.cpp \
    view/welcomepage.cpp

HEADERS += \
    model/databasemanager.h \
    model/historyrepository.h \
    model/patientrepository.h \
    model/userrepository.h \
    view/departmentpage.h \
    view/doctorpage.h \
    view/historydialog.h \
    view/loginpage.h \
    view/mainwindow.h \
    view/patientdialog.h \
    view/patientpage.h \
    view/registerdialog.h \
    view/sexdelegate.h \
    view/welcomepage.h

FORMS += \
    view/loginpage.ui

RESOURCES += \
    resources.qrc
