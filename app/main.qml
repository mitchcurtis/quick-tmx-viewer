import QtQuick 2.8
import QtQuick.Window 2.2

import org.mapeditor.Tiled 1.0 as Tiled

import App 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: "mapContainer x: " + Math.floor(mapContainer.x) + " y: " + Math.floor(mapContainer.y) + " scale: " + mapContainer.scale
//        + " click tile pos: " + Math.floor(tileCoords.x) + ", " + Math.floor(tileCoords.y)

    readonly property point mapRelativeCoords: singleFingerPanArea.mapToItem(mapItem, singleFingerPanArea.mouseX, singleFingerPanArea.mouseY)
//    readonly property point tileCoords: mapItem.screenToTileCoords(mapRelativeCoords.x, mapRelativeCoords.y)

    Tiled.MapLoader {
        id: mapLoader
        source:  "file:///" + tiledExampleDir + "/isometric_grass_and_water.tmx"
        onErrorChanged: console.warn(error)
    }

    Item {
        id: mapView
        anchors.fill: parent
        anchors.margins: 100

        Item {
            id: mapContainer

            ParallelAnimation {
                id: containerAnimation

                property alias scale: scaleAnimation.to
                property alias x: xAnimation.to
                property alias y: yAnimation.to

                NumberAnimation { id: scaleAnimation; target: mapContainer; property: "scale"; easing.type: Easing.OutCubic; duration: 100 }
                NumberAnimation { id: xAnimation; target: mapContainer; property: "x"; easing.type: Easing.OutCubic; duration: 100 }
                NumberAnimation { id: yAnimation; target: mapContainer; property: "y"; easing.type: Easing.OutCubic; duration: 100 }
            }

            Component.onCompleted: {
                containerAnimation.scale = mapContainer.scale
                containerAnimation.x = mapContainer.x
                containerAnimation.y = mapContainer.y
            }

//            Tiled.MapItem {
//                id: mapItem
//                map: mapLoader.map
//                opacity: 0.2
//                visibleArea: {
//                    var scale = mapContainer.scale
//                    Qt.rect(-mapContainer.x / scale,
//                            -mapContainer.y / scale,
//                            mapView.width / scale,
//                            mapView.height / scale);
//                }

//                Rectangle {
//                    id: player
//                    width: 32
//                    height: 32
//                    color: "darkorange"
//                    radius: width / 2
//                    z: 100
//                }
//            }

            Repeater {
                id: mapItem

                onCountChanged: print("repeater count", count)

                property rect visibleArea: model.visibleArea

                model: TiledMapModel {
                    map: mapLoader.map
                    visibleArea: {
                        var scale = mapContainer.scale
                        Qt.rect(-mapContainer.x / scale,
                                -mapContainer.y / scale,
                                mapView.width / scale,
                                mapView.height / scale);
                    }
                }

                delegate: Image {
                    x: model.tileX
                    y: model.tileY
                    z: model.tileZ
                    source: model.tileSpriteSource
                    smooth: false
                    visible: model.tileVisible
                }
            }
        }
    }

    Rectangle {
        anchors.fill: mapView
        color: "tomato"
        opacity: 0.1

        Text {
            text: "MapItem visibleArea " + mapItem.visibleArea
        }
    }

    DragArea {
        id: singleFingerPanArea
        anchors.fill: parent

//        onClicked: {
//            var relativeToMapItem = singleFingerPanArea.mapToItem(mapItem, singleFingerPanArea.mouseX, singleFingerPanArea.mouseY)
//            var tileCoords = mapItem.screenToTileCoords(relativeToMapItem.x, relativeToMapItem.y)
//            var pixelCoords = mapItem.tileToScreenCoords(Math.floor(tileCoords.x) + 0.5, Math.floor(tileCoords.y) + 0.5)
//            player.x = pixelCoords.x - player.width / 2
//            player.y = pixelCoords.y - player.height / 2
//        }

        onDragged: {
            if (containerAnimation.running) {
                containerAnimation.stop()
                containerAnimation.x += dx
                containerAnimation.y += dy
                mapContainer.x += dx
                mapContainer.y += dy
                containerAnimation.start()
            } else {
                mapContainer.x += dx
                mapContainer.y += dy
            }
        }

        onWheel: {
            var scaleFactor = Math.pow(1.4, wheel.angleDelta.y / 120)
            var scale = Math.min(8, Math.max(0.25, containerAnimation.scale * scaleFactor))
            var anchor = mapToItem(mapContainer, wheel.x, wheel.y)
            var oldScale = mapContainer.scale
            var oldX = anchor.x * oldScale
            var oldY = anchor.y * oldScale
            var newX = anchor.x * scale
            var newY = anchor.y * scale

            containerAnimation.stop()
            containerAnimation.x = mapContainer.x - (newX - oldX)
            containerAnimation.y = mapContainer.y - (newY - oldY)
            containerAnimation.scale = scale
            containerAnimation.start()
        }
    }
}
