#ifndef TILEDMAPMODEL_H
#define TILEDMAPMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QRectF>
#include <QScopedPointer>
#include <QStringList>
#include <QUrl>

#include "isometricrenderer.h"
#include "tiledquickplugin/mapref.h"

class TiledMapModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(TiledQuick::MapRef map READ map WRITE setMap NOTIFY mapChanged)
    Q_PROPERTY(QRectF visibleArea READ visibleArea WRITE setVisibleArea NOTIFY visibleAreaChanged)

public:
    enum {
        TileXRole = Qt::UserRole,
        TileYRole,
        TileZRole,
        TileSpriteSourceRole,
        TileVisibleRole,
    };

    explicit TiledMapModel(QObject *parent = 0);
    ~TiledMapModel();

    TiledQuick::MapRef map() const;
    void setMap(TiledQuick::MapRef map);

    QRectF visibleArea() const;
    void setVisibleArea(const QRectF &visibleArea);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void mapChanged();
    void visibleAreaChanged();

private:
    void repopulate();
    void updateVisibleTiles();
    QPair<int, int> layerAndTileHashIndexForRow(int row) const;

    bool isReady() const;

    struct TileData {
        int x = 0;
        int y = 0;
        int z = 0;
        QUrl source;
        bool visible = false;
    };

    TiledQuick::MapRef mMapRef;
    QRectF mVisibleArea;
    /*!
        The outer hash contains each layer, if the map has more than one.
        Some layers might be meta or might not be tile layers, which is why we use a hash and not a vector.
        The key is the index of the layer in the map's list of layers.
        The value is the tile hash (inner hash).

        The inner hash contains the tiles for that layer.
        We use a hash for the inner container because some tiles can be empty,
        and we don't want to create unused Qt Quick items for those.
        The key is the index of the tile in the layer's list of tiles.
        The value is the tile data for that tile.
    */
    typedef QHash<int, TileData> TileHash;
    QHash<int, TileHash> mTileLayers;
    // QHash::keys() is really slow, especially for the world level which is 100x100 tiles.
    // Since it its called often in data(), we cache it here.
    QHash<int, QList<int>> mCachedTileLayerKeys;
    int mTotalTileCount = 0;
    QScopedPointer<Tiled::IsometricRenderer> mRenderer;
};

#endif // TILEDMAPMODEL_H
