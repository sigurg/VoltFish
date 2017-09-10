#include <QtQuick>
#include <sailfishapp.h>
#include "qmooshimeter.h"

#define APP_VERSION "0.1"

int main(int argc, char *argv[])
{
    QGuiApplication *app = SailfishApp::application(argc, argv);
    QQuickView *view = SailfishApp::createView();
    QQmlContext *ctx = view->rootContext();

    QMooshimeter mm;
    ctx->setContextProperty("mooshimeter", &mm);
    ctx->setContextProperty("appName", "Mooshimeter");
    ctx->setContextProperty("appVersion", APP_VERSION);

    app->setQuitOnLastWindowClosed(true);

    view->setSource(SailfishApp::pathTo("/qml/Mooshimeter.qml"));
    view->showFullScreen();

    return app->exec();
}
