#-------------------------------------------------
#
# Project created by QtCreator 2017-12-01T21:34:39
#
#-------------------------------------------------

QT       += core gui widgets svg

TARGET = AA2QtEdit
TEMPLATE = app
win32 {
    QMAKE_TARGET_PRODUCT = AA2QtEdit
    QMAKE_TARGET_DESCRIPTION = Artificial Academy 2 Character Editor
    QMAKE_TARGET_COMPANY = aa2g
    QMAKE_TARGET_COPYRIGHT = AA2QtEdit is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
    RC_ICONS += icons/application-icon.ico
    VERSION = 1.8.0.0
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += src/main.cpp\
        src/ui/mainwindow.cpp \
    src/ui/cardlistview.cpp \
    src/ui/cardlistdelegate.cpp \
    src/ui/cardview.cpp \
    src/ui/carddataview.cpp \
    src/datareader.cpp \
    src/dataenumerable.cpp \
    src/datatype.cpp \
    src/datablock.cpp \
    src/cardfile.cpp \
    src/carddatamodel.cpp \
    src/dictionary.cpp \
    src/filesystemcardlistmodel.cpp \
    src/pngimage.cpp \
    src/clothdata.cpp \
    src/datastruct.cpp \
    src/classsavecardlistmodel.cpp \
    src/ui/filedialog.cpp \
    src/ui/carddatadelegate.cpp \
    src/ui/coloritemeditor.cpp \
    src/constants.cpp \
    src/cardlistmodel.cpp \
    src/filesystemcardlistmodelloader.cpp \
    src/ui/preferences.cpp \
    src/settings.cpp \
    src/ui/classsavedata.cpp

HEADERS  += src/ui/mainwindow.h \
    src/ui/cardlistview.h \
    src/ui/cardlistdelegate.h \
    src/ui/cardview.h \
    src/ui/carddataview.h \
    src/datareader.h \
    src/dataenumerable.h \
    src/datatype.h \
    src/datablock.h \
    src/cardfile.h \
    src/carddatamodel.h \
    src/dictionary.h \
    src/filesystemcardlistmodel.h \
    src/pngimage.h \
    src/clothdata.h \
    src/datastruct.h \
    src/classsavecardlistmodel.h \
    src/ui/filedialog.h \
    src/ui/carddatadelegate.h \
    src/ui/coloritemeditor.h \
    src/constants.h \
    src/cardlistmodel.h \
    src/filesystemcardlistmodelloader.h \
    src/ui/preferences.h \
    src/settings.h \
    src/ui/classsavedata.h

FORMS    += src/ui/mainwindow.ui \
    src/ui/cardview.ui \
    src/ui/preferences.ui \
    src/ui/classsavedata.ui

RESOURCES += \
    XML/datadef.qrc \
    icons/icons.qrc
