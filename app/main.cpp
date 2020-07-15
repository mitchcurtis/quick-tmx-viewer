#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <qdebug.h>

#include "RectSpriteImageProvider.h"
#include "TiledMapModel.h"

int main(int argc, char *argv[])
{
//    qputenv("QML2_IMPORT_PATH", TILED_QUICK_PLUGIN_QML_INSTALL_DIR);
    qputenv("QT_LOGGING_RULES", "lib.* = false");
    qputenv("QT_MESSAGE_PATTERN", "%{function}: %{message}");

    QGuiApplication app(argc, argv);

    qmlRegisterType<TiledMapModel>("App", 1, 0, "TiledMapModel");

    QQmlApplicationEngine engine;

    engine.addImportPath(TILED_QUICK_PLUGIN_QML_INSTALL_DIR);

    engine.addImageProvider("rectsprite", new RectSpriteImageProvider);

    engine.rootContext()->setContextProperty(QLatin1String("tiledExampleDir"), TILED_EXAMPLE_DIR);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
