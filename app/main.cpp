#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <qdebug.h>
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qDebug() << TILED_QUICK_PLUGIN_BUILD_DIR;
    engine.addImportPath(TILED_QUICK_PLUGIN_BUILD_DIR);

    engine.rootContext()->setContextProperty(QLatin1String("exampleDir"), EXAMPLE_DIR);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
