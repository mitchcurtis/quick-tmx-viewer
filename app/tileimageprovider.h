#ifndef TILEIMAGEPROVIDER_H
#define TILEIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QString>
#include <QImage>

namespace Tiled {
class Tileset;
}

class TileImageProvider : public QQuickImageProvider
{
public:
    TileImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};

#endif // TILEIMAGEPROVIDER_H
