import qbs
import qbs.FileInfo

QtGuiApplication {
    name: "app"
    targetName: "app"
    consoleApplication: false

    Depends { name: "Qt.core" }
    Depends { name: "Qt.quick" }
    // Workaround for static builds until https://bugreports.qt.io/browse/QBS-1409 is fixed
    Depends {
        name: "Qt"
        condition: Qt.core.staticBuild && qbs.targetOS.contains("linux")
        submodules: ["qxcb-egl-integration"]
    }
    Depends { name: "libtiled" }

    // Additional import path used to resolve QML modules in Qt Creator's code model
    property pathList qmlImportPaths: []

    readonly property bool darwin: qbs.targetOS.contains("darwin")
    readonly property bool unix: qbs.targetOS.contains("unix")
    readonly property bool windows: qbs.targetOS.contains("windows")

    readonly property string tiledQuickPluginDir: {
        var path
        if (windows)
            path = "/../install-root/qml"
        else if (darwin)
            path = "/../install-root/usr/local/Tiled Quick.app/Contents/qml"
        else
            path = "/../install-root/usr/local/qml"
        return FileInfo.cleanPath(buildDirectory + path)
    }

    readonly property string tiledExampleDir: FileInfo.cleanPath(sourceDirectory + "/../3rdparty/tiled/examples")

    cpp.useRPaths: darwin || (unix && !Qt.core.staticBuild)
    // Ensure that e.g. libslate is found.
    cpp.rpaths: darwin ? ["@loader_path/../Frameworks"] : ["$ORIGIN/lib"]

    cpp.cxxLanguageVersion: "c++11"
    // https://bugreports.qt.io/browse/QBS-1434
    cpp.minimumMacosVersion: "10.7"

    cpp.defines: [
        // The following define makes your compiler emit warnings if you use
        // any feature of Qt which as been marked deprecated (the exact warnings
        // depend on your compiler). Please consult the documentation of the
        // deprecated API in order to know how to port your code away from it.
        "QT_DEPRECATED_WARNINGS",

        // You can also make your code fail to compile if you use deprecated APIs.
        // In order to do so, uncomment the following line.
        // You can also select to disable deprecated APIs only up to a certain version of Qt.
        //"QT_DISABLE_DEPRECATED_BEFORE=0x060000" // disables all the APIs deprecated before Qt 6.0.0

        "TILED_EXAMPLE_DIR=\"" + tiledExampleDir + "\"",

        // TODO: find a better way
        "TILED_QUICK_PLUGIN_QML_INSTALL_DIR=\"" + tiledQuickPluginDir + "\""
    ]

    files: [
        "main.cpp",
        "qml.qrc"
    ]

    Group {
        name: "Install (non-macOS)"
        condition: !qbs.targetOS.contains("macos")
        qbs.install: true
        qbs.installSourceBase: product.buildDirectory
        fileTagsFilter: product.type
    }

    // This is necessary to install the app bundle (OS X)
    Group {
        name: "bundle.content install"
        fileTagsFilter: ["bundle.content"]
        qbs.install: true
        qbs.installDir: "."
        qbs.installSourceBase: product.buildDirectory
    }
}
