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
    adboutput.cpp \
    adbutils.cpp \
    connectadb.cpp \
    customlistwidget.cpp \
    getadbdata.cpp \
    getdevices.cpp \
    getlocaladb.cpp \
    getreturncode.cpp \
    logfile.cpp \
    mainwindow.cpp \
    about.cpp \
    helpdialog.cpp \
    returncode.cpp \
    scpdialog.cpp \
    uninstalldialog.cpp \
    preferencesdialog.cpp \
    usbfiledialog.cpp \
    adblogdialog.cpp \
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
    adbprefdialog.cpp \
    setpdialog.cpp \
    sleepdialog.cpp \
    adbstring.cpp \
    oculusdialog.cpp 



RESOURCES = adbLink.qrc

HEADERS  += mainwindow.h \
    about.h \
    adboutput.h \
    adbutils.h \
    connectadb.h \
    customlistwidget.h \
    devicerecord.h \
    getadbdata.h \
    getdevices.h \
    getlocaladb.h \
    getreturncode.h \
    helpdialog.h \
    logfile.h \
    point.h \
    returncode.h \
    scpdialog.h \
    uninstalldialog.h \
    preferencesdialog.h \
    usbfiledialog.h \
    adblogdialog.h \  
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
    adbprefdialog.h \
    sleepdialog.h \
    setpdialog.h \
    detachableprocess.h \
    adbstring.h \
    version.h \
    oculusdialog.h 
 


FORMS    +=  \
    helpdialog.ui \
    scpdialog.ui \
    uninstalldialog.ui \
    preferencesdialog.ui \
    usbfiledialog.ui \
    adblogdialog.ui \
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

macx {
    QMAKE_LIBS_OPENGL = -framework OpenGL
}
