#-------------------------------------------------
#
# Project created by QtCreator 2021-01-04T12:54:26
#
#-------------------------------------------------

QT       += core gui

CONFIG   += serialport

QMAKE_CXXFLAGS += -std=c++0x

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tool
TEMPLATE = app


SOURCES += main.cpp \
    widget.cpp \
    costum_tran/CostumTran.cpp \
    costum_tran/CostumTranReceive.cpp \
    costum_tran/CostumTranTransmit.cpp \
    costum_tran/costum_tran_ui.cpp \
    ymodem/Ymodem.cpp \
    ymodem/YmodemFileReceive.cpp \
    ymodem/YmodemFileTransmit.cpp \
    ymodem/ymodem_ui.cpp \
    info_display/infodisplay.cpp \
    info_display/modbushelper.cpp \
    options/optionsdialog.cpp \
    preference/preference.cpp \
    serialport_helper/serialporthelper.cpp

HEADERS  += widget.h \
    costum_tran/costum_tran_ui.h \
    costum_tran/CostumTran.h \
    costum_tran/CostumTranReceive.h \
    costum_tran/CostumTranTransmit.h \
    ymodem/Ymodem.h \
    ymodem/YmodemFileReceive.h \
    ymodem/YmodemFileTransmit.h \
    ymodem/ymodem_ui.h \
    info_display/infodisplay.h \
    info_display/modbushelper.h \
    options/optionsdialog.h \
    preference/preference.h \
    info_display/defs.h \
    serialport_helper/serialporthelper.h

FORMS    += widget.ui \
    costum_tran/costum_tran_ui.ui \
    ymodem/ymodem_ui.ui \
    info_display/infodisplay.ui \
    options/optionsdialog.ui

RESOURCES += \
    img.qrc

RC_FILE = tool.rc

INCLUDEPATH += costum_tran \
    ymodem \
    info_display \
    options \
    preference \
    serialport_helper \
    3rd/quc/inc \
    3rd/corevolgrp/inc \
    3rd/modbus/inc

#Link corevolgrpplugin.lib
CONFIG (debug, debug | release) {
    win32: LIBS += -L$$PWD/3rd/corevolgrp/lib -lcorevolgrpplugind
} else {
    win32: LIBS += -L$$PWD/3rd/corevolgrp/lib -lcorevolgrpplugin
}

#Link quc.lib
CONFIG (debug, debug | release) {
    win32: LIBS += -L$$PWD/3rd/quc/lib -lqucd
} else {
    win32: LIBS += -L$$PWD/3rd/quc/lib -lquc
}

#Link modbus.lib
win32: LIBS += -L$$PWD/3rd/modbus/lib -lmodbus
