/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime Mini project. Visit
* http://sourceforge.net/projects/bibletimemini for more information.
*
* This code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <QtGlobal>
#include <QtPlugin>
#include <QtDebug>

#include <QApplication>
#include <QBitmap>
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QFile>
#include <QFontDatabase>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QStringListModel>
#include <QStackedWidget>
#include <QStyle>
#include <QStyleFactory>
#include <QTextCodec>
#include <QTextStream>
#include <QTime>
#include <QTimer>
#include <QTranslator>

#include <swlog.h>
#include <filemgr.h>
#ifndef EXCLUDEZLIB
#include <zipcomprs.h>
#endif

#include "../bibletime/src/backend/config/btconfig.h"
#include "../bibletime/src/backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../bibletime/src/backend/btinstallbackend.h"
#include "../bibletime/src/backend/btinstallmgr.h"
#include "../bibletime/src/backend/cswordmodulesearch.h"
#include "../bibletime/src/backend/managers/cdisplaytemplatemgr.h"
#include "../bibletime/src/backend/managers/cswordbackend.h"
#include "../bibletime/src/backend/managers/btstringmgr.h"
#include "../bibletime/src/util/directory.h"
#include "../bibletime/src/util/cresmgr.h"
#include "../bibletime/src/frontend/bibletimeapp.h"

#include "btmini.h"
#include "models/btminimoduletextmodel.h"
#include "models/btminimodulesmodel.h"
#include "models/btminimodulenavigationmodel.h"
#include "models/btminisettingsmodel.h"
#include "ui/btminimenu.h"
#include "ui/btminipanel.h"
#include "ui/btminiui.h"
#include "view/btminiview.h"
#include "view/btminilayoutdelegate.h"

#ifdef BT_MINI_QML
#include <QtQml>
#include <QtQuick>
#endif

#if QT_VERSION >= 0x050000
#include <QAbstractNativeEventFilter>
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_ANDROID
#include <android/log.h>
#include <QtAndroidExtras>
#endif

#ifdef Q_OS_SYMBIAN
#include <hwrmvibra.h>

// required for new (std::nothrow) [] in cswordmoduleinfo.cpp
namespace std
{
const nothrow_t& GetNoThrowObj()
{
    static nothrow_t nt;
    return nt;
}
}
#endif

#ifdef Q_OS_IOS
//#include <AudioToolbox/AudioToolbox.h>
#endif

#ifdef Q_OS_BLACKBERRY
#include <bb/device/VibrationController>
#endif


#ifdef Q_OS_WINCE
#include  <windows.h>
#include  <nled.h>

extern "C"
{
    BOOL WINAPI NLedSetDevice(UINT nDeviceId, void *pInput);
};
#endif

void BtMini::vibrate(int milliseconds)
{
    if(btConfig().value<bool>("mini/disableVibration", false))
        return;

#ifdef Q_OS_WINCE
    NLED_SETTINGS_INFO settings;
    settings.LedNum = 1;
    settings.OffOnBlink = true;
    NLedSetDevice(NLED_SETTINGS_INFO_ID, &settings);

    Sleep(milliseconds);

    settings.OffOnBlink = false;
    NLedSetDevice(NLED_SETTINGS_INFO_ID, &settings);
#endif

#ifdef Q_OS_ANDROID
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/bibletimemini/MiniActivity",
                                           "vibrate", "(J)V", (jlong)milliseconds);
#endif

#ifdef Q_OS_SYMBIAN
    static bool initiated = false;
    static QScopedPointer<CHWRMVibra> v;
    if(initiated == false)
    {
        TRAPD(err, v.reset(CHWRMVibra::NewL()));
        if(err != KErrNone)
            qDebug() << "Can not create Vibra, leave:" << err;

        initiated = true;
    }

    TRAPD(err, v.data()->StartVibraL(milliseconds, 50));
    if(err != KErrNone)
        qDebug() << "Can not vibrate, leave:" << err;
#endif

#ifdef Q_OS_IOS
    //AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
#endif

#ifdef Q_OS_BLACKBERRY
    static bb::device::VibrationController v;
    if(v.isSupported())
        v.start(50, milliseconds);
#endif
}


void BtMini::keepScreenAwake(int seconds)
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/bibletimemini/MiniActivity",
                                           "keepScreenAwake", "(J)V", (jlong)seconds);
#endif
}


bool eventFilterFunction(void *message, long *result)
{
#ifdef Q_OS_WINCE
    static bool enteredMainLoop = false;
    MSG *msg = reinterpret_cast<MSG *>(message);

    Q_CHECK_PTR(msg);

    switch(msg->message)
    {
    case WM_PAINT:
        if(!enteredMainLoop)
            qDebug("Start");
        enteredMainLoop = true;
        break;
    case WM_ACTIVATE:
        if(enteredMainLoop)
        {
            int active = LOWORD(msg->wParam);
            bool minimized = (BOOL) HIWORD(msg->wParam);

            if(BtMiniUi::instance()->mainWidget()->winId() == msg->hwnd)
                BtMiniUi::instance()->worksView()->setSleep(active == WA_INACTIVE);
        }
        break;
    case WM_CLOSE: qDebug("Close"); break;
    case WM_HIBERNATE: qDebug("Low memory");break;
    }
#else
    Q_UNUSED(message);
    Q_UNUSED(result);
#endif
    return false;
}

#if QT_VERSION >= 0x050000
class EventFilterProcessor : public QAbstractNativeEventFilter
{
public:
    EventFilterProcessor() {;}

    static EventFilterProcessor* instance()
    {
        static EventFilterProcessor s;
        return &s;
    }

    bool nativeEventFilter(const QByteArray & eventType, void * message, long * result)
    {
        Q_UNUSED(eventType);
        return eventFilterFunction(message, result);
    }
};
#endif

/** Sword debug messages. */
class BtMiniSwordLog : public sword::SWLog
{
public:
    BtMiniSwordLog(){;}
    ~BtMiniSwordLog(){;}

    void logMessage(const char *message, int level) const
    {
        if(level == sword::SWLog::LOG_WARN)
            qWarning() << message;
        if(level == sword::SWLog::LOG_ERROR)
            qCritical() << message;
        else
            qDebug() << message;
    }
};

/** Debug messages. */
#if QT_VERSION < 0x050000
void BtMiniMessageHandler(QtMsgType type, const char *msg)
#else
void BtMiniMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
#endif
{
#ifdef Q_OS_SYMBIAN
    static QFile f("c:\\btlog.txt");
#else
    static QFile f("log.txt");
#endif
    static bool r = f.open(QIODevice::WriteOnly);

#if QT_VERSION < 0x050000
    QString s(QString::fromLocal8Bit(msg) + QLatin1Char('\n'));
#else
    QString s(msg + '\n');
#endif
    QTextStream (&f) << s;

#ifdef Q_OS_ANDROID
    const int a[] = {ANDROID_LOG_DEBUG, ANDROID_LOG_WARN, ANDROID_LOG_ERROR, ANDROID_LOG_FATAL};
    __android_log_print(a[qMin((const int)type, 3)], "BtMini", s.toLocal8Bit());
#elif defined Q_OS_WIN && !(defined Q_OS_WINCE)
    OutputDebugString((wchar_t*)s.utf16());
#else
    printf(s.toLocal8Bit());
#endif
}


int main(int argc, char *argv[])
{
#ifdef Q_OS_IOS
    Q_IMPORT_PLUGIN(QSvgPlugin);
#endif

#if QT_VERSION < 0x050000
    qInstallMsgHandler(BtMiniMessageHandler);
    BibleTimeApp app(argc, argv);
    app.setEventFilter(eventFilterFunction);
#else
    qInstallMessageHandler(BtMiniMessageHandler);
    BibleTimeApp app(argc, argv);
    app.installNativeEventFilter(EventFilterProcessor::instance());
#endif

    if(!util::directory::initDirectoryCache())
    {
        qFatal("Init Application: Error initializing directory cache!");
        return EXIT_FAILURE;
    }

	QFontDatabase::addApplicationFont("://jGaramond.ttf");
	QFontDatabase::addApplicationFont("jGaramond.ttf");

    app.startInit();

    // Register methatypes
    qRegisterMetaType<FilterOptions>("FilterOptions");
    qRegisterMetaType<DisplayOptions>("DisplayOptions");
    qRegisterMetaTypeStreamOperators<BtBookshelfTreeModel::Grouping>("BtBookshelfTreeModel::Grouping");

    // TODO consider removeing, but what for this was added?
    // qRegisterMetaType<BTModuleTreeItem::Grouping>("Grouping");
    // qRegisterMetaTypeStreamOperators<BTModuleTreeItem::Grouping>("Grouping");
    qRegisterMetaType<CSwordModuleSearch::SearchType>("SearchType");
    qRegisterMetaTypeStreamOperators<CSwordModuleSearch::SearchType>("SearchType");

    qRegisterMetaType<BtConfig::StringMap>("StringMap");
    qRegisterMetaTypeStreamOperators<BtConfig::StringMap>("StringMap");

    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");


    if (!app.initBtConfig())
        return EXIT_FAILURE;

    app.setStyle(btConfig().value<QString>("mini/miniStyle", "mini"));
    btConfig().setValue("bibletimeVersion", app.applicationVersion());

    // install translators
    QString locale(btConfig().value<QString>("mini/locale", QLocale::system().name()));
    qDebug() << "Select interface locale:" << locale;
    foreach(QString s, QStringList() << "bibletime_ui_" << "bibletimemini_")
    {
        QTranslator *t = new QTranslator(&app);
        t->load(s + locale, util::directory::getLocaleDir().canonicalPath());
        app.installTranslator(t);
    }

    QString errorMessage;
    new CDisplayTemplateMgr(errorMessage);
    if(!errorMessage.isNull())
    {
        qFatal(("Init Application: " + errorMessage).toLatin1());
        return -1;
    }

    // Init Sword
    sword::SWLog::setSystemLog(new BtMiniSwordLog);
    sword::SWLog::getSystemLog()->setLogLevel(btConfig().value<int>("mini/swordDebugLevel",
#ifdef QT_DEBUG
		sword::SWLog::LOG_DEBUG
#else
		sword::SWLog::LOG_ERROR
#endif
		));

    app.initIcons();

    sword::StringMgr::setSystemStringMgr(new BtStringMgr);

    CSwordBackend *backend = CSwordBackend::createInstance();
    backend->initModules(CSwordBackend::OtherChange);

#ifndef EXCLUDEZLIB
    // check locales and extract from resources, we always need locales because keys stored in localized form
    if(btConfig().value<int>("mini/swordLocalesVersion", 0) < SWORD_VERSION_NUM)
    {
        QDir d(backend->prefixPath);
        d.makeAbsolute();
        if(d.exists())
        {
            d.mkdir("locales.d");
            if(d.cd("locales.d"))
            {
                qDebug() << "Update locales to version" << SWORD_VERSION_STR;

                d.setNameFilters(QStringList() << "*.conf" << "*.tar.gz");
                d.setFilter(QDir::Files);
                foreach(QString f, d.entryList())
                    if(!d.remove(f))
                        qDebug() << "Failed to remove" << f;

                if(QFile::copy(":/locales.tar.gz", d.filePath("locales.tar.gz")))
                {
                    sword::FileDesc *fd = sword::FileMgr::getSystemFileMgr()->open(d.filePath("locales.tar.gz").toLocal8Bit(), sword::FileMgr::RDONLY);
                    int r = sword::ZipCompress::unTarGZ(
                                fd->getFd(), d.absolutePath().toLocal8Bit());
                    if(r != 0)
                        qDebug() << "Locales decopression failed:" << r;
                    sword::FileMgr::getSystemFileMgr()->close(fd);
                    if(!d.remove("locales.tar.gz"))
                        qDebug() << "Failed to remove locales.tar.gz after decompression";

                    sword::LocaleMgr::getSystemLocaleMgr()->loadConfigDir(d.absolutePath().toLocal8Bit());

                    btConfig().setValue<int>("mini/swordLocalesVersion", SWORD_VERSION_NUM);
                }
                else qDebug() << "Can't copy locales.tar.gz";
            }
            else qDebug() << "Can't cd to locales.d" << d;
        }
        else qDebug() << "Sword directory does not exists" << d;
    }
#endif

    backend->booknameLanguage(btConfig().value<QString>("language", locale));
    if(btConfig().value<bool>("deleteOrphanedIndices", true))
        backend->deleteOrphanedIndices();

    // Let's run...
#ifdef BT_MINI_QML
    QList<QUrl> files;
    files << QUrl("file:/sdcard/main.qml");
    files << QUrl("file:/D:/dev/mini__/platforms/qml/btmini/metro.qml");
    files << QUrl("file:/" + QApplication::applicationDirPath() + "/main.qml");
    files << QUrl("qrc:/bibletime/qml/metro.qml");

    qmlRegisterType<BtMiniModuleTextModel>("BibleTime.Mini", 1, 0, "SwordModuleModel");

    QQuickView view;

    foreach(QUrl s, files)
    {
        if(QFile(s.toLocalFile()).exists() || s.toString().left(4) == "qrc:")
        {
            qDebug() << "Load qml:" << s;
            view.setSource(s);
            break;
        }
    }

    view.connect(view.engine(), SIGNAL(quit()), SLOT(close()));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();
#else
    BtMiniUi::instance()->show();
#endif

    return app.exec();
}
