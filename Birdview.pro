######################################################################
# Automatically generated by qmake (3.0) Thu Dec 29 02:16:43 2016
######################################################################

QT += widgets network
# CONFIG += c++14
TEMPLATE = app
TARGET = Birdview
INCLUDEPATH += .
QMAKE_CXXFLAGS += -Wfatal-errors
DEFINES += QCUSTOMPLOT_USE_LIBRARY
LIBS += -lqcustomplot-qt5

# Input
HEADERS += src/Birdview.hpp \
           src/ConnectDialog.hpp
SOURCES += src/Birdview.cpp \
           src/ConnectDialog.cpp \
           src/main.cpp
