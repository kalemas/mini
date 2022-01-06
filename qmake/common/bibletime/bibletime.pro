# BibleTime project for QtCreator
#   to run this target you must copy "share" folder from original BibleTime
#   installation to parent folder of executeale location

# Configuration
VERSION = 3.0.2
CONFIG += \
    clucene \
    svg \
    xml\


QT += \
    printsupport \
    qml \
    quickwidgets\


DEFINES += BT_RUNTIME_DOCDIR=\\\"share/doc\\\"
debug:DEFINES += BT_DEBUG


SOURCES += \
    ../../../bibletime/src/backend/btsourcesthread.cpp \
    ../../../bibletime/src/backend/models/btlistmodel.cpp \
    ../../../bibletime/src/backend/models/btmoduletextmodel.cpp \
    ../../../bibletime/src/frontend/bibletime_init.cpp \
    ../../../bibletime/src/frontend/bibletime_slots.cpp \
    ../../../bibletime/src/frontend/bibletime.cpp \
    ../../../bibletime/src/frontend/bibletimeapp.cpp \
    ../../../bibletime/src/frontend/BookmarkItem.cpp \
    ../../../bibletime/src/frontend/bookmarks/bteditbookmarkdialog.cpp \
    ../../../bibletime/src/frontend/bookmarks/cbookmarkindex.cpp \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfinstallfinalpage.cpp \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelflanguagespage.cpp \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfremovefinalpage.cpp \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfsourcespage.cpp \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfsourcesprogresspage.cpp \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelftaskpage.cpp \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfwizardpage.cpp \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfworkspage.cpp \
    ../../../bibletime/src/frontend/bookshelfwizard/btinstallpagemodel.cpp \
    ../../../bibletime/src/frontend/bookshelfwizard/cswordsetupinstallsourcesdialog.cpp \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfwizard.cpp \
    ../../../bibletime/src/frontend/btaboutdialog.cpp \
    ../../../bibletime/src/frontend/btaboutmoduledialog.cpp \
    ../../../bibletime/src/frontend/btbookshelfdockwidget.cpp \
    ../../../bibletime/src/frontend/btbookshelfgroupingmenu.cpp \
    ../../../bibletime/src/frontend/btbookshelfview.cpp \
    ../../../bibletime/src/frontend/btbookshelfwidget.cpp \
    ../../../bibletime/src/frontend/btcopybyreferencesdialog.cpp \
    ../../../bibletime/src/frontend/btmenuview.cpp \
    ../../../bibletime/src/frontend/btmessageinputdialog.cpp \
    ../../../bibletime/src/frontend/BtMimeData.cpp \
    ../../../bibletime/src/frontend/btmodulechooserdialog.cpp \
    ../../../bibletime/src/frontend/btmoduleindexdialog.cpp \
    ../../../bibletime/src/frontend/btopenworkaction.cpp \
    ../../../bibletime/src/frontend/btprinter.cpp \
    ../../../bibletime/src/frontend/bttextbrowser.cpp \
    ../../../bibletime/src/frontend/bturlhandler.cpp \
    ../../../bibletime/src/frontend/cexportmanager.cpp \
    ../../../bibletime/src/frontend/cinfodisplay.cpp \
    ../../../bibletime/src/frontend/cmdiarea.cpp \
    ../../../bibletime/src/frontend/display/btcolorwidget.cpp \
    ../../../bibletime/src/frontend/display/btfindwidget.cpp \
    ../../../bibletime/src/frontend/display/btfontsizewidget.cpp \
    ../../../bibletime/src/frontend/display/btmodelviewreaddisplay.cpp \
    ../../../bibletime/src/frontend/display/cdisplay.cpp \
    ../../../bibletime/src/frontend/display/creaddisplay.cpp \
    ../../../bibletime/src/frontend/display/modelview/btqmlinterface.cpp \
    ../../../bibletime/src/frontend/display/modelview/btqmlscrollview.cpp \
    ../../../bibletime/src/frontend/display/modelview/btquickwidget.cpp \
    ../../../bibletime/src/frontend/display/modelview/bttextfilter.cpp \
    ../../../bibletime/src/frontend/displaywindow/btactioncollection.cpp \
    ../../../bibletime/src/frontend/displaywindow/btdisplaysettingsbutton.cpp \
    ../../../bibletime/src/frontend/displaywindow/btmodulechooserbar.cpp \
    ../../../bibletime/src/frontend/displaywindow/btmodulechooserbutton.cpp \
    ../../../bibletime/src/frontend/displaywindow/btmodulechoosermenu.cpp \
    ../../../bibletime/src/frontend/displaywindow/bttextwindowheader.cpp \
    ../../../bibletime/src/frontend/displaywindow/bttextwindowheaderwidget.cpp \
    ../../../bibletime/src/frontend/displaywindow/bttoolbarpopupaction.cpp \
    ../../../bibletime/src/frontend/displaywindow/cbiblereadwindow.cpp \
    ../../../bibletime/src/frontend/displaywindow/cbookreadwindow.cpp \
    ../../../bibletime/src/frontend/displaywindow/ccommentaryreadwindow.cpp \
    ../../../bibletime/src/frontend/displaywindow/cdisplaywindow.cpp \
    ../../../bibletime/src/frontend/displaywindow/clexiconreadwindow.cpp \
    ../../../bibletime/src/frontend/displaywindow/creadwindow.cpp \
    ../../../bibletime/src/frontend/edittextwizard/btedittextpage.cpp \
    ../../../bibletime/src/frontend/edittextwizard/btedittextwizard.cpp \
    ../../../bibletime/src/frontend/edittextwizard/btplainorhtmlpage.cpp \
    ../../../bibletime/src/frontend/keychooser/bthistory.cpp \
    ../../../bibletime/src/frontend/keychooser/cbookkeychooser.cpp \
    ../../../bibletime/src/frontend/keychooser/cbooktreechooser.cpp \
    ../../../bibletime/src/frontend/keychooser/ckeychooser.cpp \
    ../../../bibletime/src/frontend/keychooser/ckeychooserwidget.cpp \
    ../../../bibletime/src/frontend/keychooser/clexiconkeychooser.cpp \
    ../../../bibletime/src/frontend/keychooser/cscrollbutton.cpp \
    ../../../bibletime/src/frontend/keychooser/cscrollerwidgetset.cpp \
    ../../../bibletime/src/frontend/keychooser/versekeychooser/btbiblekeywidget.cpp \
    ../../../bibletime/src/frontend/keychooser/versekeychooser/btdropdownchooserbutton.cpp \
    ../../../bibletime/src/frontend/keychooser/versekeychooser/btversekeymenu.cpp \
    ../../../bibletime/src/frontend/keychooser/versekeychooser/cbiblekeychooser.cpp \
    ../../../bibletime/src/frontend/main.cpp \
    ../../../bibletime/src/frontend/messagedialog.cpp \
    ../../../bibletime/src/frontend/searchdialog/analysis/csearchanalysisdialog.cpp \
    ../../../bibletime/src/frontend/searchdialog/analysis/csearchanalysisitem.cpp \
    ../../../bibletime/src/frontend/searchdialog/analysis/csearchanalysislegenditem.cpp \
    ../../../bibletime/src/frontend/searchdialog/analysis/csearchanalysisscene.cpp \
    ../../../bibletime/src/frontend/searchdialog/analysis/csearchanalysisview.cpp \
    ../../../bibletime/src/frontend/searchdialog/btindexdialog.cpp \
    ../../../bibletime/src/frontend/searchdialog/btsearchmodulechooserdialog.cpp \
    ../../../bibletime/src/frontend/searchdialog/btsearchoptionsarea.cpp \
    ../../../bibletime/src/frontend/searchdialog/btsearchresultarea.cpp \
    ../../../bibletime/src/frontend/searchdialog/btsearchsyntaxhelpdialog.cpp \
    ../../../bibletime/src/frontend/searchdialog/chistorycombobox.cpp \
    ../../../bibletime/src/frontend/searchdialog/cmoduleresultview.cpp \
    ../../../bibletime/src/frontend/searchdialog/crangechooserdialog.cpp \
    ../../../bibletime/src/frontend/searchdialog/csearchdialog.cpp \
    ../../../bibletime/src/frontend/searchdialog/csearchresultview.cpp \
    ../../../bibletime/src/frontend/settingsdialogs/btconfigdialog.cpp \
    ../../../bibletime/src/frontend/settingsdialogs/btfontsettings.cpp \
    ../../../bibletime/src/frontend/settingsdialogs/btshortcutsdialog.cpp \
    ../../../bibletime/src/frontend/settingsdialogs/btshortcutseditor.cpp \
    ../../../bibletime/src/frontend/settingsdialogs/btstandardworkstab.cpp \
    ../../../bibletime/src/frontend/settingsdialogs/bttextfilterstab.cpp \
    ../../../bibletime/src/frontend/settingsdialogs/cacceleratorsettings.cpp \
    ../../../bibletime/src/frontend/settingsdialogs/cconfigurationdialog.cpp \
    ../../../bibletime/src/frontend/settingsdialogs/cdisplaysettings.cpp \
    ../../../bibletime/src/frontend/settingsdialogs/clistwidget.cpp \
    ../../../bibletime/src/frontend/settingsdialogs/cswordsettings.cpp \
    ../../../bibletime/src/frontend/tips/bttipdialog.cpp \
    ../../../bibletime/src/frontend/welcome/btwelcomedialog.cpp \
    ../../../bibletime/src/util/btmodules.cpp \


HEADERS += \
    ../../../bibletime/src/backend/btsourcesthread.h \
    ../../../bibletime/src/backend/models/btlistmodel.h \
    ../../../bibletime/src/backend/models/btmoduletextmodel.h \
    ../../../bibletime/src/frontend/bibletime.h \
    ../../../bibletime/src/frontend/bibletimeapp.h \
    ../../../bibletime/src/frontend/BookmarkItem.h \
    ../../../bibletime/src/frontend/bookmarks/bteditbookmarkdialog.h \
    ../../../bibletime/src/frontend/bookmarks/cbookmarkindex.h \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfinstallfinalpage.h \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelflanguagespage.h \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfremovefinalpage.h \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfsourcespage.h \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfsourcesprogresspage.h \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelftaskpage.h \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfwizard.h \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfwizardpage.h \
    ../../../bibletime/src/frontend/bookshelfwizard/btbookshelfworkspage.h \
    ../../../bibletime/src/frontend/bookshelfwizard/btinstallpagemodel.h \
    ../../../bibletime/src/frontend/bookshelfwizard/cswordsetupinstallsourcesdialog.h \
    ../../../bibletime/src/frontend/btaboutdialog.h \
    ../../../bibletime/src/frontend/btaboutmoduledialog.h \
    ../../../bibletime/src/frontend/btbookshelfdockwidget.h \
    ../../../bibletime/src/frontend/btbookshelfgroupingmenu.h \
    ../../../bibletime/src/frontend/btbookshelfview.h \
    ../../../bibletime/src/frontend/btbookshelfwidget.h \
    ../../../bibletime/src/frontend/btcopybyreferencesdialog.h \
    ../../../bibletime/src/frontend/btmenuview.h \
    ../../../bibletime/src/frontend/btmessageinputdialog.h \
    ../../../bibletime/src/frontend/BtMimeData.h \
    ../../../bibletime/src/frontend/btmodulechooserdialog.h \
    ../../../bibletime/src/frontend/btmoduleindexdialog.h \
    ../../../bibletime/src/frontend/btopenworkaction.h \
    ../../../bibletime/src/frontend/btprinter.h \
    ../../../bibletime/src/frontend/bttextbrowser.h \
    ../../../bibletime/src/frontend/bturlhandler.h \
    ../../../bibletime/src/frontend/cexportmanager.h \
    ../../../bibletime/src/frontend/cinfodisplay.h \
    ../../../bibletime/src/frontend/cmdiarea.h \
    ../../../bibletime/src/frontend/display/btcolorwidget.h \
    ../../../bibletime/src/frontend/display/btfindwidget.h \
    ../../../bibletime/src/frontend/display/btfontsizewidget.h \
    ../../../bibletime/src/frontend/display/btmodelviewreaddisplay.h \
    ../../../bibletime/src/frontend/display/cdisplay.h \
    ../../../bibletime/src/frontend/display/creaddisplay.h \
    ../../../bibletime/src/frontend/display/modelview/btqmlinterface.h \
    ../../../bibletime/src/frontend/display/modelview/btqmlscrollview.h \
    ../../../bibletime/src/frontend/display/modelview/btquickwidget.h \
    ../../../bibletime/src/frontend/display/modelview/bttextfilter.h \
    ../../../bibletime/src/frontend/displaywindow/btactioncollection.h \
    ../../../bibletime/src/frontend/displaywindow/btdisplaysettingsbutton.h \
    ../../../bibletime/src/frontend/displaywindow/btmodulechooserbar.h \
    ../../../bibletime/src/frontend/displaywindow/btmodulechooserbutton.h \
    ../../../bibletime/src/frontend/displaywindow/btmodulechoosermenu.h \
    ../../../bibletime/src/frontend/displaywindow/bttextwindowheader.h \
    ../../../bibletime/src/frontend/displaywindow/bttextwindowheaderwidget.h \
    ../../../bibletime/src/frontend/displaywindow/bttoolbarpopupaction.h \
    ../../../bibletime/src/frontend/displaywindow/btwindowmodulechooser.h \
    ../../../bibletime/src/frontend/displaywindow/cbiblereadwindow.h \
    ../../../bibletime/src/frontend/displaywindow/cbookreadwindow.h \
    ../../../bibletime/src/frontend/displaywindow/ccommentaryreadwindow.h \
    ../../../bibletime/src/frontend/displaywindow/cdisplaywindow.h \
    ../../../bibletime/src/frontend/displaywindow/clexiconreadwindow.h \
    ../../../bibletime/src/frontend/displaywindow/creadwindow.h \
    ../../../bibletime/src/frontend/edittextwizard/btedittextpage.h \
    ../../../bibletime/src/frontend/edittextwizard/btedittextwizard.h \
    ../../../bibletime/src/frontend/edittextwizard/btplainorhtmlpage.h \
    ../../../bibletime/src/frontend/keychooser/bthistory.h \
    ../../../bibletime/src/frontend/keychooser/cbookkeychooser.h \
    ../../../bibletime/src/frontend/keychooser/cbooktreechooser.h \
    ../../../bibletime/src/frontend/keychooser/ckeychooser.h \
    ../../../bibletime/src/frontend/keychooser/ckeychooserwidget.h \
    ../../../bibletime/src/frontend/keychooser/clexiconkeychooser.h \
    ../../../bibletime/src/frontend/keychooser/cscrollbutton.h \
    ../../../bibletime/src/frontend/keychooser/cscrollerwidgetset.h \
    ../../../bibletime/src/frontend/keychooser/versekeychooser/btbiblekeywidget.h \
    ../../../bibletime/src/frontend/keychooser/versekeychooser/btdropdownchooserbutton.h \
    ../../../bibletime/src/frontend/keychooser/versekeychooser/btversekeymenu.h \
    ../../../bibletime/src/frontend/keychooser/versekeychooser/cbiblekeychooser.h \
    ../../../bibletime/src/frontend/searchdialog/analysis/csearchanalysisdialog.h \
    ../../../bibletime/src/frontend/searchdialog/analysis/csearchanalysisitem.h \
    ../../../bibletime/src/frontend/searchdialog/analysis/csearchanalysislegenditem.h \
    ../../../bibletime/src/frontend/searchdialog/analysis/csearchanalysisscene.h \
    ../../../bibletime/src/frontend/searchdialog/analysis/csearchanalysisview.h \
    ../../../bibletime/src/frontend/searchdialog/btindexdialog.h \
    ../../../bibletime/src/frontend/searchdialog/btsearchmodulechooserdialog.h \
    ../../../bibletime/src/frontend/searchdialog/btsearchoptionsarea.h \
    ../../../bibletime/src/frontend/searchdialog/btsearchresultarea.h \
    ../../../bibletime/src/frontend/searchdialog/btsearchsyntaxhelpdialog.h \
    ../../../bibletime/src/frontend/searchdialog/chistorycombobox.h \
    ../../../bibletime/src/frontend/searchdialog/cmoduleresultview.h \
    ../../../bibletime/src/frontend/searchdialog/crangechooserdialog.h \
    ../../../bibletime/src/frontend/searchdialog/csearchdialog.h \
    ../../../bibletime/src/frontend/searchdialog/csearchresultview.h \
    ../../../bibletime/src/frontend/settingsdialogs/btconfigdialog.h \
    ../../../bibletime/src/frontend/settingsdialogs/btfontsettings.h \
    ../../../bibletime/src/frontend/settingsdialogs/btshortcutsdialog.h \
    ../../../bibletime/src/frontend/settingsdialogs/btshortcutseditor.h \
    ../../../bibletime/src/frontend/settingsdialogs/btstandardworkstab.h \
    ../../../bibletime/src/frontend/settingsdialogs/bttextfilterstab.h \
    ../../../bibletime/src/frontend/settingsdialogs/cacceleratorsettings.h \
    ../../../bibletime/src/frontend/settingsdialogs/cconfigurationdialog.h \
    ../../../bibletime/src/frontend/settingsdialogs/cdisplaysettings.h \
    ../../../bibletime/src/frontend/settingsdialogs/clistwidget.h \
    ../../../bibletime/src/frontend/settingsdialogs/cswordsettings.h \
    ../../../bibletime/src/frontend/tips/bttipdialog.h \
    ../../../bibletime/src/frontend/welcome/btwelcomedialog.h \
    ../../../bibletime/src/util/btmodules.h \


RESOURCES += \
    ../../../bibletime/src/frontend/display/modelview/modelviewqml.qrc \


OTHER_FILES += \


# Windows platfrom
windows {
DEFINES += NO_DBUS
}

# BibleTime Core
include(../../common/core/core.pro)

# show translations in project explorer
OTHER_FILES += $${TRANSLATIONS}
