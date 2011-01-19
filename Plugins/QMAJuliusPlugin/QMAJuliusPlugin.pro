TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../..
TARGET = $$qtLibraryTarget(QMAJuliusPlugin)
DESTDIR = ../plugins

unix {
    LIBS += -L/usr/local/lib -lMMDAI -lMMDFiles -ljulius -lsent -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lz
    INCLUDEPATH += /usr/include/bullet /usr/local/include/bullet /usr/local/include
}
macx {
    # depends on libjulis
    LIBS += -framework AudioToolbox -framework AudioUnit -framework CoreAudio -framework CoreServices
}

HEADERS += \
    QMAJuliusPlugin.h \
    Julius_Thread.h \
    QMAJuliusInitializer.h

SOURCES += \
    QMAJuliusPlugin.cc \
    Julius_Thread.cpp \
    QMAJuliusInitializer.cc
