#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QQuickItem>
#include <QString>

class QQmlComponent;

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
};

#endif // MAPVIEW_H
