#include <QtQuick>
#include <sailfishapp.h>
#include "ble-scanner.h"
#include "qmooshimeter.h"

#define APP_VERSION "0.2"

int main(int argc, char *argv[])
{
    QGuiApplication *app = SailfishApp::application(argc, argv);
    QQuickView *view = SailfishApp::createView();
    QQmlContext *ctx = view->rootContext();

    QMooshimeter mm;
    ctx->setContextProperty("mooshimeter", &mm);

    BLEScanner ble;
    ctx->setContextProperty("blescanner", &ble);

    ctx->setContextProperty("appName", "VoltFish");
    ctx->setContextProperty("appVersion", APP_VERSION);

    app->setQuitOnLastWindowClosed(true);

    view->setSource(SailfishApp::pathTo("/qml/VoltFish.qml"));
    view->showFullScreen();

    return app->exec();
}
