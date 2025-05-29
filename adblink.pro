#-------------------------------------------------
#
# Project created by QtCreator 2014-05-21T01:43:07
#
#-------------------------------------------------

QT       += core gui sql network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#LIBS += -lQt5Concurrent

TARGET = adblink
TEMPLATE = app

SOURCES += main.cpp\
    adbutils.cpp \
    connectadb.cpp \
    customlistwidget.cpp \
    deviceinfodialog.cpp \
    getadbdata.cpp \
    getlocaladb.cpp \
    getreturncode.cpp \
    logfile.cpp \
    mainwindow.cpp \
    about.cpp \
    helpdialog.cpp \
    scpdialog.cpp \
    uninstalldialog.cpp \
    preferencesdialog.cpp \
    usbfiledialog.cpp \
    adblogdialog.cpp \
    kodidialog.cpp \
    datadialog.cpp \
    restdialog.cpp \
    backupdialog.cpp \
    cachedialog.cpp \
    keyboarddialog.cpp \
    listfiledialog.cpp \
    klogdialog.cpp \
    oslogdialog.cpp \
    editordialog.cpp \
    searchdialog.cpp \
    tcpipdialog.cpp \
    forcequitdialog.cpp \
    pushfiledialog.cpp \
    adbprefdialog.cpp \
    setpdialog.cpp \
    sleepdialog.cpp \
    adbstring.cpp \
    oculusdialog.cpp 



RESOURCES = adbLink.qrc

HEADERS  += mainwindow.h \
    about.h \
    adbutils.h \
    connectadb.h \
    customlistwidget.h \
    deviceinfodialog.h \
    devicerecord.h \
    getadbdata.h \
    getlocaladb.h \
    getreturncode.h \
    helpdialog.h \
    logfile.h \
    scpdialog.h \
    uninstalldialog.h \
    preferencesdialog.h \
    usbfiledialog.h \
    adblogdialog.h \  
    kodidialog.h \
    datadialog.h \
    restdialog.h \
    listfiledialog.h \
    backupdialog.h \
    cachedialog.h \
    keyboarddialog.h \
    klogdialog.h \
    oslogdialog.h \
    editordialog.h \
    searchdialog.h \
    tcpipdialog.h \    
    forcequitdialog.h \
    pushfiledialog.h \
    adbprefdialog.h \
    sleepdialog.h \
    version.h \
    setpdialog.h \
    detachableprocess.h \
    adbstring.h \
    oculusdialog.h 
 


FORMS    += mainwindow.ui \
    about.ui \
    deviceinfodialog.ui \
    helpdialog.ui \
    scpdialog.ui \
    uninstalldialog.ui \
    preferencesdialog.ui \
    usbfiledialog.ui \
    adblogdialog.ui \
    kodidialog.ui \
    datadialog.ui \
    restdialog.ui \
    backupdialog.ui \
    cachedialog.ui \
    listfiledialog.ui \
    keyboarddialog.ui \
    klogdialog.ui \
    oslogdialog.ui \
    editordialog.ui \
    searchdialog.ui \
    tcpipdialog.ui \
    forcequitdialog.ui \
    pushfiledialog.ui \
    adbprefdialog.ui \
    setpdialog.ui \
    sleepdialog.ui\
    oculusdialog.ui


QMAKE_CXXFLAGS += -Wno-deprecated

ICON = adblink.icns
RC_FILE = adblink.rc

# QMAKE_APPLE_DEVICE_ARCHS=arm64

# QMAKE_MAC_SDK = macosx12.3

# QMAKE_MAC_SDK = macosx13.0

linux {
QMAKE_LFLAGS += -no-pie
}

QMAKE_CXXFLAGS += -Wno-deprecated-declarations

CONFIG+=sdk_no_version_check

OTHER_FILES +=

qmake

INCLUDEPATH += $$HOME/qtbuild


