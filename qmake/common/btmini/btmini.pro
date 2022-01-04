# Configuration
VERSION = 1.3.0

CONFIG += clucene svg xml mini


# Base section, nothing platform specific
DEFINES += BT_MINI
DEFINES += BT_MINI_VERSION=\\\"$${VERSION}\\\"


INCLUDEPATH += . \
    ../../../bibletime/src/frontend-mini \


SOURCES += \
    ../../../bibletime/src/bibletimeapp.cpp \
    ../../../bibletime/src/frontend-mini/btmini.cpp \
    ../../../bibletime/src/frontend-mini/models/btminimodulenavigationmodel.cpp \
    ../../../bibletime/src/frontend-mini/models/btminimodulesmodel.cpp \
    ../../../bibletime/src/frontend-mini/models/btminimoduletextmodel.cpp \
    ../../../bibletime/src/frontend-mini/models/btminisettingsmodel.cpp \
    ../../../bibletime/src/frontend-mini/view/btminilayoutdelegate.cpp \
    ../../../bibletime/src/frontend-mini/view/btminiview.cpp \
    ../../../bibletime/src/frontend-mini/ui/btminimenu.cpp \
    ../../../bibletime/src/frontend-mini/ui/btminipanel.cpp \
    ../../../bibletime/src/frontend-mini/ui/btministyle.cpp \
    ../../../bibletime/src/frontend-mini/ui/btminiui.cpp \
    ../../../bibletime/src/frontend-mini/ui/btminiworkswidget.cpp \
    ../../../bibletime/src/frontend-mini/ui/btminiclippingswidget.cpp


HEADERS += \
    ../../../bibletime/src/bibletimeapp.h \
    ../../../bibletime/src/frontend-mini/btmini.h \
    ../../../bibletime/src/frontend-mini/models/btminimodulesmodel.h \
    ../../../bibletime/src/frontend-mini/models/btminimodulenavigationmodel.h \
    ../../../bibletime/src/frontend-mini/models/btminimoduletextmodel.h \
    ../../../bibletime/src/frontend-mini/models/btminisettingsmodel.h \
    ../../../bibletime/src/frontend-mini/view/btminiview.h \
    ../../../bibletime/src/frontend-mini/view/btminilayoutdelegate.h \
    ../../../bibletime/src/frontend-mini/ui/btminimenu.h \
    ../../../bibletime/src/frontend-mini/ui/btminipanel.h \
    ../../../bibletime/src/frontend-mini/ui/btministyle.h \
    ../../../bibletime/src/frontend-mini/ui/btminiui.h \
    ../../../bibletime/src/frontend-mini/ui/btminiworkswidget.h \
    ../../../bibletime/src/frontend-mini/ui/btminiwidget.h \
    ../../../bibletime/src/frontend-mini/ui/btminiclippingswidget.h


RESOURCES += \
    ../../../btmini.qrc \
    ../../../bibletime/src/frontend-mini/ui/btministyle.qrc \


OTHER_FILES += \
    ../../../bibletime/src/display-templates/Basic-Mini.tmpl \
    ../../../bibletime/src/frontend-mini/todo.txt \


# Translation
TRANSLATIONS += \
    ../../../bibletime/i18n/messages/bibletime_ui_ru.ts \
    ../../../bibletime/src/frontend-mini/translations/bibletimemini_ru.ts \


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
SOURCES += ../../../bibletime/src/frontend-mini/view/btstatictext.cpp
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
