QT += gui core websockets widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += serialport gui

TEMPLATE = lib
CONFIG += c++14
DEFINES += SMARTHOME_LIBRARY

release: TARGET = smarthome
debug: TARGET = smarthomed

DESTDIR = $$(DEV_LIBS)

release: BUILDDIR = build/release
debug:   BUILDDIR = build/debug

OBJECTS_DIR = $$BUILDDIR/.obj
MOC_DIR = $$BUILDDIR/.moc
RCC_DIR = $$BUILDDIR/.qrc
UI_DIR = $$BUILDDIR/.ui

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBPATH = $$(DEV_LIBS)

if(debug){
LIBS += -L$$LIBPATH -ltagsystemd
}
else{
    LIBS += -L$$LIBPATH -ltagsystem
}

INCLUDEPATH += $$(JUNE_ROOT)

if(debug){
    LIBS += -L$$(DEV_LIBS) -ltagsystemd
}
else{
    LIBS += -L$$(DEV_LIBS) -ltagsystem
}





SOURCES += \
    smarthome.cpp \
    tplinkcloud.cpp \
    tplinksmartsocket.cpp \
    hs110.cpp


HEADERS += \
    smarthome.h \
    smarthome_global.h \
    tplinkcloud.h \
    tplinksmartsocket.h \
    hs110.h
