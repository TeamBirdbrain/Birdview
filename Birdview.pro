######################################################################
# Automatically generated by qmake (3.0) Thu Dec 29 02:16:43 2016
######################################################################

QT += widgets network svg printsupport
CONFIG += c++14
TEMPLATE = app
TARGET = Birdview
RESOURCES = Birdview.qrc
INCLUDEPATH += .
!win32:QMAKE_CXXFLAGS += -Wfatal-errors

# Input
HEADERS += src/Birdview.hpp \
           src/ConnectDialog.hpp \
           qcustomplot/qcustomplot.h
SOURCES += src/Birdview.cpp \
           src/ConnectDialog.cpp \
           src/main.cpp \
           qcustomplot/qcustomplot.cpp
