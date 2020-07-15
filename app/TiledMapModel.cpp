#include "TiledMapModel.h"

#include <QLoggingCategory>
#include <QScopeGuard>

#include "map.h"
#include "tilelayer.h"

Q_LOGGING_CATEGORY(lcTiledMapModel, "lib.tiledMapModel")

TiledMapModel::TiledMapModel(QObject *parent) :
    QAbstractListModel(parent)
{
    qCDebug(lcTiledMapModel) << this;
}

TiledMapModel::~TiledMapModel()
{
    qCDebug(lcTiledMapModel) << this;
}

TiledQuick::MapRef TiledMapModel::map() const
{
    return mMapRef;
}

void TiledMapModel::setMap(TiledQuick::MapRef map)
{
    if (mMapRef == map)
        return;

    qCDebug(lcTiledMapModel) << "set map" << map.mMap;
    mMapRef = map;
    mRenderer.reset(new Tiled::IsometricRenderer(mMapRef.mMap));
    repopulate();

    emit mapChanged();
}

QRectF TiledMapModel::visibleArea() const
{
    return mVisibleArea;
}

void TiledMapModel::setVisibleArea(const QRectF &visibleArea)
{
    if (visibleArea == mVisibleArea)
        return;

    mVisibleArea = visibleArea;
    updateVisibleTiles();

    emit visibleAreaChanged();
}

int TiledMapModel::rowCount(const QModelIndex &) const
{
    return mMapRef.mMap && mRenderer ? mTotalTileCount : 0;
}

QVariant TiledMapModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid))
        return QVariant();

    // We present ourselves to QML as a flattened list, which is why there is a bit of work to do here.
    const QPair<int, int> layerAndTileHashIndices = layerAndTileHashIndexForRow(index.row());
    const auto tileLayerHash = mTileLayers.value(layerAndTileHashIndices.first);
    const auto cachedTileLayerHashKeys = mCachedTileLayerKeys.value(layerAndTileHashIndices.first);
    const int tileIndex = cachedTileLayerHashKeys.at(layerAndTileHashIndices.second);
    auto tileData = tileLayerHash.value(tileIndex);

    switch (role) {
    case TileXRole: return tileData.x;
    case TileYRole: return tileData.y;
    case TileZRole: return tileData.z;
    case TileSpriteSourceRole: return tileData.source;
    case TileVisibleRole: return tileData.visible;
    }

    return QVariant();
}

QHash<int, QByteArray> TiledMapModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[TileXRole] = "tileX";
    names[TileYRole] = "tileY";
    names[TileZRole] = "tileZ";
    names[TileSpriteSourceRole] = "tileSpriteSource";
    names[TileVisibleRole] = "tileVisible";
    return names;
}

/*!
    This should only be called when the level changes.

    It is designed to be called before and after level changes so that the tiles are
    cleared when a level is exiting, and populated when it is entering.
*/
void TiledMapModel::repopulate()
{
    qCDebug(lcTiledMapModel) << "repopulate called with map:" << mMapRef.mMap << "visibleArea:" << mVisibleArea;

    qCDebug(lcTiledMapModel) << "about to reset model";
    beginResetModel();

    auto resetModel = qScopeGuard([=] {
        endResetModel();
        qCDebug(lcTiledMapModel) << "reset model";
    });

    mTileLayers.clear();
    mCachedTileLayerKeys.clear();
    mTotalTileCount = 0;

    if (!mMapRef.mMap || !mRenderer) {
        qCDebug(lcTiledMapModel) << "model cannot repopulate yet";
        return;
    }

    auto tmxMap = mMapRef.mMap;
    qCDebug(lcTiledMapModel) << "repopulating TiledMapModel for level \"test\" with map" << tmxMap << "...";

    const int layerCount = tmxMap->layerCount();
    for (int i = 0; i < layerCount; ++i) {
        auto layer = tmxMap->layerAt(i);
        if (!layer->isTileLayer())
            continue;

        auto tileLayer = static_cast<Tiled::TileLayer*>(layer);
        if (tileLayer->name() == QLatin1String("meta"))
            continue;

        auto &tiles = mTileLayers[i];

        qCDebug(lcTiledMapModel) << "- fetching data for layer" << tileLayer->name() << "at index" << i;

        // We iterate through all tiles in the layer, but only consider those that aren't empty.
        const int layerWidth = tileLayer->width();
        const int layerHeight = tileLayer->height();
        for (int y = 0; y < layerHeight; ++y) {
            for (int x = 0; x < layerWidth; ++x) {
                // x, y, z and visible are set in updateVisibleTiles().
                TileData tileData;

                auto cell = tileLayer->cellAt(x, y);
                if (cell.isEmpty())
                    continue;

                QString tilesetImagePath = cell.tileset()->imageSourceString();
//                tilesetImagePath.replace(QLatin1String("qrc:/"), QString());

                // These values are based on TilesetHelper::setTileset() and TilesetHelper::setTextureCoordinates().
                auto tileset = cell.tileset();
                const int tileSpacing = tileset->tileSpacing();
                const int margin = tileset->margin();
                const int tileHSpace = tileset->tileWidth() + tileSpacing;
                const int tileVSpace = tileset->tileHeight() + tileSpacing;
                const QSize tilesetSize(tileset->imageWidth(), tileset->imageHeight());
                const int availableWidth = tilesetSize.width() + tileSpacing - margin;
                const int tilesPerRow = qMax(availableWidth / tileHSpace, 1);

                const int tileId = cell.tileId();
                const int column = tileId % tilesPerRow;
                const int row = tileId / tilesPerRow;
                const int tx = column * tileHSpace + margin;
                const int ty = row * tileVSpace + margin;

                QString sourceStr = QString::fromLatin1("image://rectsprite/%1,%2,%3,%4,%5")
                    .arg(tilesetImagePath).arg(tx).arg(ty).arg(tileset->tileWidth()).arg(tileset->tileHeight());

                if (cell.flippedHorizontally())
                    sourceStr += QStringLiteral(",fh");
                if (cell.flippedVertically())
                    sourceStr += QStringLiteral(",fv");

                tileData.source = QUrl(sourceStr);

                const int tileIndex = y * layerWidth + x;
                tiles[tileIndex] = tileData;
                ++mTotalTileCount;
            }
        }

        mCachedTileLayerKeys[i] = tiles.keys();
    }

    qCDebug(lcTiledMapModel) << "... populated model with" << mTotalTileCount << "tiles over" << mTileLayers.size() << "layers";
}

void TiledMapModel::updateVisibleTiles()
{
    qCDebug(lcTiledMapModel) << "updateVisibleTiles called with map:" << mMapRef.mMap << "renderer:" << mRenderer << "visibleArea:" << mVisibleArea;

    if (!isReady()) {
        qCDebug(lcTiledMapModel) << "cannot update visbile tiles yet";
        return;
    }

    qCDebug(lcTiledMapModel) << "updating visible tiles TiledMapModel for level" << "test" << "...";

    auto tmxMap = mMapRef.mMap;
    const int layerCount = tmxMap->layerCount();
    for (int i = 0; i < layerCount; ++i) {
        auto layer = tmxMap->layerAt(i);
        if (!layer->isTileLayer())
            continue;

        auto tileLayer = static_cast<Tiled::TileLayer*>(layer);
        if (tileLayer->name() == QLatin1String("meta"))
            continue;

        auto &tiles = mTileLayers[i];
        auto hiddenTiles = tiles;
        const auto tileKeys = mCachedTileLayerKeys.value(i);

        // ... and let the renderer do the hard work of figuring out which ones are visible.
        auto renderCallback = [&](const Tiled::Cell &/*cell*/, const QPoint &tilePos, const QPointF &screenPos, const QSizeF &/*size*/) {
            const int tileIndex = tilePos.y() * tileLayer->width() + tilePos.x();

            TileData &tileData = tiles[tileIndex];
            if (tileData.z == 0) {
                // We would do this in repopulate(), but we need the screen position from the renderer.
                // TODO: IsometricRenderer::drawTileLayer()'s render callback passes CellRenderer::BottomLeft to
                // CellRenderer::render()... and we are clearly missing something because our tiles are rendered
                // too far to the bottom-right. So we adjust it here with some hard-coded values that I don't understand for now...
                tileData.x = screenPos.x();
                tileData.y = screenPos.y() - 48;
                const int largestCoord = qMax(tilePos.x(), tilePos.y());
                const int smallestCoord = qMin(tilePos.x(), tilePos.y());
                tileData.z = largestCoord + smallestCoord;

                const int keyIndex = tileKeys.indexOf(tileIndex);
                const QModelIndex modelIndex = createIndex(keyIndex, 0);
                dataChanged(modelIndex, modelIndex, { TileXRole, TileYRole, TileZRole });
            }
            // TODO: call setData with visible role
            const bool changed = !tileData.visible;
            tileData.visible = true;
            if (changed) {
                const int keyIndex = tileKeys.indexOf(tileIndex);
                const QModelIndex modelIndex = createIndex(keyIndex, 0);
                dataChanged(modelIndex, modelIndex, { TileVisibleRole });
            }

            // This tile is now visible, so we can remove it from our list of tiles that should be hidden.
            hiddenTiles.remove(tileIndex);
        };

        qCDebug(lcTiledMapModel) << "- updating visible tiles by \"rendering\" layer" << tileLayer->name() << "at index" << i;
        mRenderer->drawTileLayer(tileLayer, renderCallback, mVisibleArea);

        qCDebug(lcTiledMapModel) << "- updating hidden tiles";
        const auto hiddenTileKeys = hiddenTiles.keys();
        for (const auto hiddenTileIndex : hiddenTileKeys) {
            const auto tile = hiddenTiles.value(hiddenTileIndex);
            if (!tile.visible)
                continue; // Already hidden; nothing to do.

            tiles[hiddenTileIndex].visible = false;

            const int keyIndex = tileKeys.indexOf(hiddenTileIndex);
            const QModelIndex modelIndex = createIndex(keyIndex, 0);
            dataChanged(modelIndex, modelIndex, { TileVisibleRole });
        }

        qCDebug(lcTiledMapModel).nospace() << "  ... updated visibility of " << tiles.size() << " tiles ("
            << tiles.size() - hiddenTiles.size() << " visible, " << hiddenTiles.size() << " hidden)";
    }

    qCDebug(lcTiledMapModel) << "... finished updating visibility of tiles";
}

QPair<int, int> TiledMapModel::layerAndTileHashIndexForRow(int row) const
{
    Q_ASSERT_X(row >= 0 && row < mTotalTileCount, Q_FUNC_INFO,
        qPrintable(QString::fromLatin1("Row %1 is out of bounds").arg(row)));

    // Find the layer and tile index by going through each layer and decreasing
    // the remaining tiles by the tile count for each layer as we go.
    int remainingTiles = row;
    const auto tileLayerValues = mTileLayers.values();
    for (int layerIndex = 0; layerIndex < mTileLayers.size(); ++layerIndex) {
        const auto tileLayer = tileLayerValues.at(layerIndex);
        if (remainingTiles < tileLayer.size())
            return { layerIndex, remainingTiles };

        remainingTiles -= tileLayer.size();
    }

    Q_UNREACHABLE();
}

bool TiledMapModel::isReady() const
{
    return mMapRef.mMap && mRenderer && !mVisibleArea.isEmpty();
}
