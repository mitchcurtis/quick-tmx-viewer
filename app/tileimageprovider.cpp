#include "tileimageprovider.h"

#include <QImage>
#include <QDebug>

#include "libtiled/tile.h"
#include "libtiled/tileset.h"
#include "libtiled/tilesetmanager.h"

TileImageProvider::TileImageProvider() :
    QQuickImageProvider(QQmlImageProviderBase::Image)
{
}

QImage TileImageProvider::requestImage(const QString &id, QSize *size, const QSize &)
{
    QStringList args = id.split(QLatin1String(","));
    if (args.length() < 2) {
        qWarning() << "Must pass two arguments as image id";
        return QImage();
    }

    const QString tilesetFilename = args.first();
    const QString tileIdStr = args.last();
    bool convertedToIntSuccessfully = false;
    const int tileId = tileIdStr.toInt(&convertedToIntSuccessfully);
    if (!convertedToIntSuccessfully) {
        qWarning() << "Failed to convert tile id" << tileIdStr << "to an int";
        return QImage();
    }

    Tiled::TilesetManager *tilesetManager = Tiled::TilesetManager::instance();
    Tiled::SharedTileset tileset = tilesetManager->findTileset(tilesetFilename);
    if (!tileset) {
        qWarning() << "No tileset at file name" << tilesetFilename;
        return QImage();
    }

    Tiled::Tile *tile = tileset->tileAt(tileId);
    if (!tile) {
        qWarning() << "No tile in tileset" << tilesetFilename << "with id" << tileId;
        return QImage();
    }

    QImage tileImage = tile->image().toImage();
    if (tileImage.isNull()) {
        qWarning() << "Tile image from tileset" << tilesetFilename << "with id" << tileId << "is null";
        return QImage();
    }

    *size = tileImage.size();
    return tileImage;
}
