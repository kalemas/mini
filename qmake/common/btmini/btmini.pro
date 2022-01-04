# Configuration
VERSION = 1.3.0

CONFIG += clucene svg xml mini


# Base section, nothing platform specific
DEFINES += BT_MINI
DEFINES += BT_MINI_VERSION=\\\"$${VERSION}\\\"


INCLUDEPATH += . \
    ../../../frontend \


SOURCES += \
    ../../../bibletime/src/frontend/bibletimeapp.cpp \
    ../../../bibletime/src/frontend/messagedialog.cpp \
    ../../../frontend/btmini.cpp \
    ../../../frontend/models/btminimodulenavigationmodel.cpp \
    ../../../frontend/models/btminimodulesmodel.cpp \
    ../../../frontend/models/btminimoduletextmodel.cpp \
    ../../../frontend/models/btminisettingsmodel.cpp \
    ../../../frontend/ui/btminiclippingswidget.cpp
    ../../../frontend/ui/btminimenu.cpp \
    ../../../frontend/ui/btminipanel.cpp \
    ../../../frontend/ui/btministyle.cpp \
    ../../../frontend/ui/btminiui.cpp \
    ../../../frontend/ui/btminiworkswidget.cpp \
    ../../../frontend/view/btminilayoutdelegate.cpp \
    ../../../frontend/view/btminiview.cpp \


HEADERS += \
    ../../../bibletime/src/frontend/bibletimeapp.h \
    ../../../frontend/btmini.h \
    ../../../frontend/models/btminimodulenavigationmodel.h \
    ../../../frontend/models/btminimodulesmodel.h \
    ../../../frontend/models/btminimoduletextmodel.h \
    ../../../frontend/models/btminisettingsmodel.h \
    ../../../frontend/ui/btminiclippingswidget.h
    ../../../frontend/ui/btminimenu.h \
    ../../../frontend/ui/btminipanel.h \
    ../../../frontend/ui/btministyle.h \
    ../../../frontend/ui/btminiui.h \
    ../../../frontend/ui/btminiwidget.h \
    ../../../frontend/ui/btminiworkswidget.h \
    ../../../frontend/view/btminilayoutdelegate.h \
    ../../../frontend/view/btminiview.h \


RESOURCES += \
    ../../../btmini.qrc \
    ../../../frontend/ui/btministyle.qrc \


OTHER_FILES += \
    ../../../frontend/mini-display-template.tmpl \
    ../../../frontend/todo.txt \


# Translation
TRANSLATIONS += \
    ../../../bibletime/i18n/messages/bibletime_ui_ru.ts \
    ../../../frontend/translations/bibletimemini_ru.ts \


# Android platform
android {
!lessThan(QT_MAJOR_VERSION, 5):!lessThan(QT_MINOR_VERSION, 2):QT += androidextras
}


# Windows platform
windows {
}

# iOS Platform
mac {
DEFINES -= BT_MINI_VERSION=\\\"$${VERSION}\\\"
DEFINES += BT_MINI_VERSION="\\\\\"$${VERSION}\\\\\""

LIBS += -framework AudioToolbox
}

# MeeGo platform
unix:contains(MEEGO_EDITION,harmattan) {

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
}


# Symbian platform
symbian {
# version
DEFINES -= BT_MINI_VERSION=\\\"$${VERSION}\\\"
greaterThan(S60_VERSION, 5.0) {
DEFINES += BT_MINI_VERSION=\"$${VERSION}\"
}
else {
DEFINES += BT_MINI_VERSION=\"\\\"$${VERSION}\\\"\"
}

LIBS += -lhwrmvibraclient

# icon
ICON += btmini.svg
DEPLOYMENT += ICON

TARGET.UID3 = 0xE5723167 # Developer UID, Store is 0x2007174E
TARGET.CAPABILITY += NetworkServices
#TARGET.EPOCSTACKSIZE = 0x200000
TARGET.EPOCHEAPSIZE = 0x040000 0x4000000

DEPLOYMENT.display_name = BibleTime Mini

packageheader = "$${LITERAL_HASH}{\"BibleTime Mini\"}, ($${TARGET.UID3}), $$replace(VERSION, ([.]\\d+), ""), \
    $$replace(VERSION, (^\\d+.)|(.\\d+$), ""), $$replace(VERSION, (\\d+[.]), ""), TYPE=SA"

vendorinfo = \
"%{\"Crosswire\"}" \
":\"Crosswire\""

mini_deployment.pkg_prerules = packageheader vendorinfo

DEPLOYMENT += mini_deployment
}


# Windows Mobile Platform
wince {
DEFINES += BT_STATIC_TEXT
SOURCES += ../../../frontend/view/btstatictext.cpp
}


# BlackBerry10 Platform
blackberry {
LIBS += -lbbdevice
}


# Windows Phone
winphone {
# have to be built against curl
}


# QtQuick definitions, maybe separate into different project?
qml:DEFINES += BT_MINI_QML
qml:QT += qml quick


# BibleTime Core
include(../../common/core/core.pro)


# show translations in project explorer
OTHER_FILES += $${TRANSLATIONS}
