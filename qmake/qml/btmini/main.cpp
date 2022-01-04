#include <QtWidgets>
#include <QtQml>
#include <QtQuick>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQuickView view;

    QList<QUrl> files;
    files << QUrl("file:/sdcard/main.qml");
    files << QUrl("file:/" + QApplication::applicationDirPath() + "/main.qml");
    files << QUrl("qrc:/bibletime/qml/metro.qml");

    foreach(QUrl s, files)
    {
        if(QFile(s.toLocalFile()).exists() || s.toString().left(4) == "qrc:")
        {
            view.setSource(s);
            break;
        }
    }

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();

    return app.exec();
}
