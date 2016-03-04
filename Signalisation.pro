QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Signalisation-BDDM-master
TEMPLATE = app


SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/cercledetection.cpp

HEADERS  += src/mainwindow.h \
    src/cercledetection.h

FORMS    += mainwindow.ui
