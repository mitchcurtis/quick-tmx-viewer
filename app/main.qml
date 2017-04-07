import QtQuick 2.4
import QtQuick.Window 2.2

import App 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    MapView {
        id: mapView
        source: "file:///" + exampleDir + "/isometric_grass_and_water.tmx"
        anchors.fill: parent
        tileComponent: Image {
            smooth: false
            asynchronous: true
        }
    }
}
