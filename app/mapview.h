#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QQuickItem>
#include <QString>

class QQmlComponent;

namespace Tiled {
class Map;
}

class MapView : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QQmlComponent *tileComponent READ tileComponent WRITE setTileComponent NOTIFY tileComponentChanged)
public:
    MapView();

    QUrl source() const;
    void setSource(const QUrl &source);

    QQmlComponent *tileComponent() const;
    void setTileComponent(QQmlComponent *tileComponent);

signals:
    void sourceChanged();
    void tileComponentChanged();

public slots:

private:
    void loadMap();

    void createIsometricItems();

    QUrl mSource;
    QQmlComponent *mTileComponent;
    Tiled::Map *mMap;
};

#endif // MAPVIEW_H
