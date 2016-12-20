QT       += core gui
QT      += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrfEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    trf.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    trf.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
