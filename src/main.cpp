#include <QtQuick>
#include <sailfishapp.h>
#include "qmooshimeter.h"

#define APP_VERSION "0.1.1"

// pages names for pull-down menu
__attribute__((unused)) constexpr auto PAGE_NAMES{
    QT_TRANSLATE_NOOP("MainPage", "About"),
    QT_TRANSLATE_NOOP("MainPage", "Settings")
};

int main(int argc, char *argv[])
{
    QGuiApplication *app = SailfishApp::application(argc, argv);
    QQuickView *view = SailfishApp::createView();
    QQmlContext *ctx = view->rootContext();

    QMooshimeter mm;
    ctx->setContextProperty("mooshimeter", &mm);
    ctx->setContextProperty("appName", "VoltFish");
    ctx->setContextProperty("appVersion", APP_VERSION);

    app->setQuitOnLastWindowClosed(true);

    view->setSource(SailfishApp::pathTo("/qml/VoltFish.qml"));
    view->showFullScreen();

    return app->exec();
}
