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
        ./../TheTradingMachine/ \
        ./../Algorithms/

SOURCES += \
        main.cpp \
        qcustomplot.cpp \
    thetradingmachinemainwindow.cpp \
    playdialog.cpp \
    thetradingmachinetab.cpp \
    CandleMaker.cpp

HEADERS += \
        qcustomplot.h \
    thetradingmachinemainwindow.h \
    ../TheTradingMachine/TheTradingMachine.h \
    playdialog.h \
    thetradingmachinetab.h \
    ../IBInterfaceClient/IBInterfaceClient.h \
    CandleMaker.h

FORMS += \
    thetradingmachinemainwindow.ui \
    playdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc \
    resource.qrc

win32: LIBS += -L$$PWD/../x64/Debug/ -lTheTradingMachine

INCLUDEPATH += $$PWD/../x64/Debug
DEPENDPATH += $$PWD/../x64/Debug
