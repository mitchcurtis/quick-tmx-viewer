#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "mapview.h"
#include "tileimageprovider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<MapView>("App", 1, 0, "MapView");

    QQmlApplicationEngine engine;

    TileImageProvider *tileImageProvider = new TileImageProvider;
    engine.addImageProvider(QLatin1String("tile"), tileImageProvider);

    engine.rootContext()->setContextProperty(QLatin1String("exampleDir"), EXAMPLE_DIR);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
