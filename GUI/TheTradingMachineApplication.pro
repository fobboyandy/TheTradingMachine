#-------------------------------------------------
#
# Project created by QtCreator 2018-09-16T00:39:25
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = TheTradingMachineApplication
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

INCLUDEPATH += \
        ./../InteractiveBrokersClient/InteractiveBrokersClient/

SOURCES += \
        main.cpp \
        qcustomplot.cpp \
    thetradingmachinemainwindow.cpp \
    playdialog.cpp \
    thetradingmachinetab.cpp \
    CandleMaker.cpp \
    candlevolumeplot.cpp

HEADERS += \
        qcustomplot.h \
    thetradingmachinemainwindow.h \
    playdialog.h \
    thetradingmachinetab.h \
    CandleMaker.h \
    ../BaseAlgorithm/BaseAlgorithm/PlotData.h \
    ../Indicators/Indicators/Common.h \
    ../Indicators/Indicators/Indicator.h \
    ../Indicators/Indicators/SimpleMovingAverage.h \
    iplot.h \
    indicatorplot.h \
    candlevolumeplot.h

FORMS += \
    thetradingmachinemainwindow.ui \
    playdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

win32: LIBS += -L$$PWD/../InteractiveBrokersClient/x64/Debug/ -lInteractiveBrokersClient

INCLUDEPATH += $$PWD/../InteractiveBrokersClient/x64/Debug
DEPENDPATH += $$PWD/../InteractiveBrokersClient/x64/Debug

win32: LIBS += -L$$PWD/../Indicators/x64/Debug/ -lIndicators

INCLUDEPATH += $$PWD/../Indicators/x64/Debug
DEPENDPATH += $$PWD/../Indicators/x64/Debug
