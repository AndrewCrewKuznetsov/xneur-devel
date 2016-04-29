QT += core gui dbus

TARGET = kdeneur
TEMPLATE = app
SEPARATOR = -
VERSION = 0.18.0

CONFIG += dbus

include(i18n/localize.pri)

SOURCES += \
    main.cpp \
    kxneurtray.cpp \
    frmabout.cpp \
    frmsettings.cpp \
    kdeneur.cpp \
    xkb.c \
    frmaddabbreviature.cpp \
    xneurconfig.cpp \
    getnameapp.cpp \
    ruleschange.cpp \
    addrules.cpp \
    edithotkey.cpp \
    adduseraction.cpp

INCLUDEPATH += /usr/include

LIBS += -lX11 -lkdeui -lkdecore -lxneur -lxnconfig -lplasma

HEADERS += \
    kxneurtray.h \
    frmabout.h \
    frmsettings.h \
    tabbar.h \
    kdeneur.h \
    xkb.h \
    frmaddabbreviature.h \
    xneurconfig.h \
    getnameapp.h \
    ruleschange.h \
    addrules.h \
    edithotkey.h \
    adduseraction.h

FORMS += \
    frmabout.ui \
    frmsettings.ui \
    frmaddabbreviature.ui \
    getnameapp.ui \
    ruleschange.ui \
    addrules.ui \
    edithotkey.ui \
    adduseraction.ui

RESOURCES += \
    resursrc.qrc



unix{
 isEmpty(PREFIX) {
   PREFIX = /usr
 }
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share
LOCALEDIR = $$DATADIR/$${TARGET}/i18n
SHAREDIR = $$DATADIR/$${TARGET}

 contains(QMAKE_HOST.arch, x86_64) {
    DEFINES += XNEUR_PLUGIN_DIR=\\\"/usr/lib/xneur\\\"
 }
 else {
    DEFINES += XNEUR_PLUGIN_DIR=\\\"/usr/lib/xneur\\\"
 }

DEFINES += PACKAGE_PIXMAPS_DIR=\\\"$${SHAREDIR}/pixmaps\\\" SHAREDIR=\\\"$${SHAREDIR}\\\" LOCALEDIR=\\\"$${LOCALEDIR}\\\"

INSTALLS =  target \
            flags \
            iconsvg \
            desktop \
            translate \
            icon_x16 \
            icon_x22 \
            icon_x24 \
            icon_x32 \
            icon_x48

target.path    = $${BINDIR}

flags.files  += pixmaps/png/*
flags.path  = $${SHAREDIR}/pixmaps

iconsvg.path = $$DATADIR/icons/hicolor/scalable/apps
iconsvg.files += pixmaps/scalable/*

desktop.path = $$DATADIR/applications
desktop.files += ../kdeneur.desktop

translate.path = $$LOCALEDIR
translate.files += i18n/*.qm

icon_x16.path = $$DATADIR/icons/hicolor/16x16/apps
icon_x16.files += pixmaps/x16/*

icon_x22.path = $$DATADIR/icons/hicolor/22x22/apps
icon_x22.files += pixmaps/x22/*

icon_x24.path = $$DATADIR/icons/hicolor/24x24/apps
icon_x24.files += pixmaps/x24/*

icon_x32.path = $$DATADIR/icons/hicolor/32x32/apps
icon_x32.files += pixmaps/x32/*

icon_x48.path = $$DATADIR/icons/hicolor/48x48/apps
icon_x48.files += pixmaps/x48/*

}

OTHER_FILES +=
