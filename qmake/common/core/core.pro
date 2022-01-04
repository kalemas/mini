# BibleTime Core, could be used for Desktop, Mini and Mobile
BT_VERSION = 2.10.0_rc1
DEFINES += BT_VERSION=\\\"$${BT_VERSION}\\\"


# Useless warnings
gcc:QMAKE_CXXFLAGS_DEBUG += -Wno-switch -Wno-unused-parameter \
    -Wno-unused-variable -Wno-reorder -Wno-missing-field-initializers \
    -fpermissive  # TEMP


INCLUDEPATH += ../../../bibletime/src


SOURCES += \
    ../../../bibletime/src/backend/bookshelfmodel/btbookshelffiltermodel.cpp \
    ../../../bibletime/src/backend/bookshelfmodel/btbookshelfmodel.cpp \
    ../../../bibletime/src/backend/bookshelfmodel/btbookshelftreemodel.cpp \
    ../../../bibletime/src/backend/bookshelfmodel/moduleitem.cpp \
    ../../../bibletime/src/backend/bookshelfmodel/languageitem.cpp \
    ../../../bibletime/src/backend/bookshelfmodel/item.cpp \
    ../../../bibletime/src/backend/bookshelfmodel/categoryitem.cpp \
    ../../../bibletime/src/backend/bookshelfmodel/indexingitem.cpp \
    ../../../bibletime/src/backend/btbookmarksmodel.cpp \
    ../../../bibletime/src/backend/btinstallbackend.cpp \
    ../../../bibletime/src/backend/btinstallmgr.cpp \
    ../../../bibletime/src/backend/btinstallthread.cpp \
    ../../../bibletime/src/backend/btmoduletreeitem.cpp \
    ../../../bibletime/src/backend/config/btconfig.cpp \
    ../../../bibletime/src/backend/config/btconfigcore.cpp \
    ../../../bibletime/src/backend/cswordmodulesearch.cpp \
    ../../../bibletime/src/backend/drivers/cswordbiblemoduleinfo.cpp \
    ../../../bibletime/src/backend/drivers/cswordbookmoduleinfo.cpp \
    ../../../bibletime/src/backend/drivers/cswordcommentarymoduleinfo.cpp \
    ../../../bibletime/src/backend/drivers/cswordlexiconmoduleinfo.cpp \
    ../../../bibletime/src/backend/drivers/cswordmoduleinfo.cpp \
    ../../../bibletime/src/backend/filters/gbftohtml.cpp \
    ../../../bibletime/src/backend/filters/osistohtml.cpp \
    ../../../bibletime/src/backend/filters/plaintohtml.cpp \
    ../../../bibletime/src/backend/filters/teitohtml.cpp \
    ../../../bibletime/src/backend/filters/thmltohtml.cpp \
    ../../../bibletime/src/backend/keys/cswordkey.cpp \
    ../../../bibletime/src/backend/keys/cswordldkey.cpp \
    ../../../bibletime/src/backend/keys/cswordtreekey.cpp \
    ../../../bibletime/src/backend/keys/cswordversekey.cpp \
    ../../../bibletime/src/backend/managers/btstringmgr.cpp \
    ../../../bibletime/src/backend/managers/cdisplaytemplatemgr.cpp \
    ../../../bibletime/src/backend/managers/clanguagemgr.cpp \
    ../../../bibletime/src/backend/managers/cswordbackend.cpp \
    ../../../bibletime/src/backend/managers/referencemanager.cpp \
    ../../../bibletime/src/backend/rendering/btinforendering.cpp \
    ../../../bibletime/src/backend/rendering/cbookdisplay.cpp \
    ../../../bibletime/src/backend/rendering/cchapterdisplay.cpp \
    ../../../bibletime/src/backend/rendering/cdisplayrendering.cpp \
    ../../../bibletime/src/backend/rendering/centrydisplay.cpp \
    ../../../bibletime/src/backend/rendering/chtmlexportrendering.cpp \
    ../../../bibletime/src/backend/rendering/cplaintextexportrendering.cpp \
    ../../../bibletime/src/backend/rendering/ctextrendering.cpp \
    ../../../bibletime/src/btglobal.cpp \
    ../../../bibletime/src/util/bticons.cpp \
    ../../../bibletime/src/util/cresmgr.cpp \
    ../../../bibletime/src/util/directory.cpp \
    ../../../bibletime/src/util/tool.cpp \


HEADERS += \
    ../../../bibletime/src/backend/bookshelfmodel/btbookshelffiltermodel.h \
    ../../../bibletime/src/backend/bookshelfmodel/btbookshelfmodel.h \
    ../../../bibletime/src/backend/bookshelfmodel/btbookshelftreemodel.h \
    ../../../bibletime/src/backend/bookshelfmodel/categoryitem.h \
    ../../../bibletime/src/backend/bookshelfmodel/indexingitem.h \
    ../../../bibletime/src/backend/bookshelfmodel/item.h \
    ../../../bibletime/src/backend/bookshelfmodel/languageitem.h \
    ../../../bibletime/src/backend/bookshelfmodel/moduleitem.h \
    ../../../bibletime/src/backend/btbookmarksmodel.h \
    ../../../bibletime/src/backend/btinstallbackend.h \
    ../../../bibletime/src/backend/btinstallmgr.h \
    ../../../bibletime/src/backend/btinstallthread.h \
    ../../../bibletime/src/backend/btmoduletreeitem.h \
    ../../../bibletime/src/backend/config/btconfig.h \
    ../../../bibletime/src/backend/config/btconfigcore.h \
    ../../../bibletime/src/backend/cswordmodulesearch.h \
    ../../../bibletime/src/backend/drivers/cswordbiblemoduleinfo.h \
    ../../../bibletime/src/backend/drivers/cswordbookmoduleinfo.h \
    ../../../bibletime/src/backend/drivers/cswordcommentarymoduleinfo.h \
    ../../../bibletime/src/backend/drivers/cswordlexiconmoduleinfo.h \
    ../../../bibletime/src/backend/drivers/cswordmoduleinfo.h \
    ../../../bibletime/src/backend/filters/gbftohtml.h \
    ../../../bibletime/src/backend/filters/osistohtml.h \
    ../../../bibletime/src/backend/filters/plaintohtml.h \
    ../../../bibletime/src/backend/filters/teitohtml.h \
    ../../../bibletime/src/backend/filters/thmltohtml.h \
    ../../../bibletime/src/backend/managers/btstringmgr.h \
    ../../../bibletime/src/backend/managers/cdisplaytemplatemgr.h \
    ../../../bibletime/src/backend/managers/clanguagemgr.h \
    ../../../bibletime/src/backend/managers/cswordbackend.h \
    ../../../bibletime/src/backend/managers/referencemanager.h \
    ../../../bibletime/src/backend/keys/cswordkey.h \
    ../../../bibletime/src/backend/keys/cswordldkey.h \
    ../../../bibletime/src/backend/keys/cswordtreekey.h \
    ../../../bibletime/src/backend/keys/cswordversekey.h \
    ../../../bibletime/src/backend/rendering/cbookdisplay.h \
    ../../../bibletime/src/backend/rendering/cchapterdisplay.h \
    ../../../bibletime/src/backend/rendering/cdisplayrendering.h \
    ../../../bibletime/src/backend/rendering/centrydisplay.h \
    ../../../bibletime/src/backend/rendering/chtmlexportrendering.h \
    ../../../bibletime/src/backend/rendering/cplaintextexportrendering.h \
    ../../../bibletime/src/backend/rendering/ctextrendering.h \
    ../../../bibletime/src/util/btsignal.h \
    ../../../bibletime/src/util/cresmgr.h \
    ../../../bibletime/src/util/directory.h \
    ../../../bibletime/src/util/tool.h \


# Core Platform Section

# iOS Platform
mac:CONFIG -= webkit

# Android platform
android {
!lessThan(QT_MAJOR_VERSION, 5):CONFIG -= webkit
DEFINES += STDC_HEADERS
}

# Symbian platform
# on S60 webkit not works, maybe wrong packaging?
symbian {
DEFINES -= BT_VERSION=\\\"$${BT_VERSION}\\\"
greaterThan(S60_VERSION, 5.0) {
DEFINES += BT_VERSION=\"$${BT_VERSION}\"
}
else {
DEFINES += BT_VERSION=\"\\\"$${BT_VERSION}\\\"\"
CONFIG -= webkit
}
}

# BlackBerry10 Platform
blackberry {
CONFIG -= webkit
DEFINES += unix
LIBS += -lsocket
}

# Qt
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
svg:QT += svg xml


# Core Configuration Section

# WebKit
# should be after platforms section, optional
webkit {
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += webkitwidgets
}
else {
    QT += webkit
}
DEFINES += BT_MINI_WEBKIT
}
else:!mini:!mobile {
warning("Non Mini build: WebKit required")
}

# Includes
clucene:include(../../common/clucene/clucene.pro)
!clucene:DEFINES += BT_NO_LUCENE

curl:include(../../common/curl/curl.pro)  # optional
icu:include(../../common/icu/icu.pro)  # optional
include(../../common/sword/sword.pro)
include(../../common/zlib/zlib.pro)
