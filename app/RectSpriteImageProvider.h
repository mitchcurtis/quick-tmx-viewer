#ifndef RECTSPRITEIMAGEPROVIDER_H
#define RECTSPRITEIMAGEPROVIDER_H

#include <QHash>
#include <QImage>
#include <QString>
#include <QQuickImageProvider>

class RectSpriteImageProvider : public QQuickImageProvider
{
public:
    RectSpriteImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    int parseInt(const QString &intStr) const;

    QHash<QString, QImage> mImages;
};

#endif // RECTSPRITEIMAGEPROVIDER_H
