QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Signalisation-BDDM-master
TEMPLATE = app

DESTDIR = $$PWD

app_data.path    = $${DESTDIR}
app_data.files   = $$PWD/data

INSTALLS += app_data

win32:DEPLOY_COMMAND    = windeployqt
macx:DEPLOY_COMMAND     = macdeployqt


SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/cercledetection.cpp \
    src/filesbddm.cpp \
    src/affichage.cpp \
    src/indexationrecherche.cpp

HEADERS  += src/mainwindow.h \
    src/cercledetection.h \
    src/filesbddm.h \
    src/affichage.h \
    src/indexationrecherche.h

FORMS    += mainwindow.ui
