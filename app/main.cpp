#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <qdebug.h>
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

//    qputenv("QML2_IMPORT_PATH", TILED_QUICK_PLUGIN_QML_INSTALL_DIR);

    QQmlApplicationEngine engine;

    engine.addImportPath(TILED_QUICK_PLUGIN_QML_INSTALL_DIR);

    engine.rootContext()->setContextProperty(QLatin1String("tiledExampleDir"), TILED_EXAMPLE_DIR);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
