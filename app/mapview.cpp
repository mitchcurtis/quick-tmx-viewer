#include "mapview.h"

#include "libtiled/layer.h"
#include "libtiled/map.h"
#include "libtiled/mapreader.h"
#include "libtiled/tilelayer.h"
#include "libtiled/tilesetmanager.h"

#include <QQmlComponent>

MapView::MapView() :
    mTileComponent(nullptr),
    mMap(nullptr)
{
}

QUrl MapView::source() const
{
    return mSource;
}

void MapView::setSource(const QUrl &source)
{
    if (mSource == source)
        return;

    mSource = source;
    loadMap();
    emit sourceChanged();
}

void MapView::loadMap()
{
    Tiled::MapReader mapReader;
    mMap = mapReader.readMap(mSource.toLocalFile());
    if (!mMap) {
        qWarning() << "Failed to read map at" << mSource.toLocalFile() << ":" << mapReader.errorString();
        return;
    }

    Tiled::TilesetManager::instance()->addReferences(mMap->tilesets());

    if (mMap->orientation() == Tiled::Map::Isometric) {
        createIsometricItems();
    } else {
        qDebug() << "Only isometric maps are currently supported";
    }
}

void MapView::createIsometricItems()
{
    for (int layerIndex = 0; layerIndex < mMap->layerCount(); ++layerIndex) {
        Tiled::Layer *layer = mMap->layerAt(layerIndex);
        // TODO: draw other stuff
        Tiled::TileLayer *tileLayer = layer->asTileLayer();

        const int startX = (tileLayer->bounds().width() * mMap->tileWidth()) / 2;
        const int startY = 0;
        for (int y = 0; y < tileLayer->bounds().height(); ++y) {
            for (int x = 0; x < tileLayer->bounds().width(); ++x) {
                const Tiled::Cell cell = tileLayer->cellAt(x, y);
                QQuickItem *tileItem = qobject_cast<QQuickItem*>(mTileComponent->beginCreate(qmlContext(this)));
                if (tileItem) {
                    QString source = QString::fromLatin1("image://tile/%1,%2").arg(cell.tileset()->fileName()).arg(cell.tileId());
                    tileItem->setProperty("source", source);
                    const int cellX = startX + (-y * (mMap->tileWidth() / 2)) + (x * (mMap->tileWidth() / 2));
                    tileItem->setX(cellX);
                    const int cellY = startY + (y * (mMap->tileHeight() / 2)) + (x * (mMap->tileHeight() / 2));
                    tileItem->setY(cellY);
                    tileItem->setParentItem(this);
                    mTileComponent->completeCreate();
                }
            }
        }
    }
}

QQmlComponent *MapView::tileComponent() const
{
    return mTileComponent;
}

void MapView::setTileComponent(QQmlComponent *tileComponent)
{
    if (mTileComponent == tileComponent)
        return;

    mTileComponent = tileComponent;
    emit tileComponentChanged();
}
