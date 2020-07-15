#include "RectSpriteImageProvider.h"

#include <QImage>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcRectSpriteImageProvider, "lib.rectSpriteImageProvider")

RectSpriteImageProvider::RectSpriteImageProvider() :
    QQuickImageProvider(QQmlImageProviderBase::Image)
{
}

static const QRgb whiteRgba = qRgba(255, 255, 255, 255);
static const QRgb magentaRgba = qRgba(255, 0, 255, 255);
static const QRgb transparentRgba = qRgba(0, 0, 0, 0);

QImage RectSpriteImageProvider::requestImage(const QString &id, QSize *size, const QSize &)
{
    const QStringList args = id.split(QLatin1String(","));
    if (args.length() < 5) {
        qWarning() << "Must pass file name, x, y, width and height as arguments for RectSpriteImageProvider source";
        return QImage();
    }

    const QString imageFilename = args.first();

    const int rectX = parseInt(args.at(1));
    if (rectX == -1)
        return QImage();

    const int rectY = parseInt(args.at(2));
    if (rectY == -1)
        return QImage();

    const int rectWidth = parseInt(args.at(3));
    if (rectWidth == -1)
        return QImage();

    const int rectHeight = parseInt(args.at(4));
    if (rectHeight == -1)
        return QImage();

    bool flipHorizontally = false;
    bool flipVertically = false;
    if (args.length() > 5) {
        const auto sixthArg = args.at(5);
        if (sixthArg == "fh")
            flipHorizontally = true;
        else if (sixthArg == "fv")
            flipVertically = true;
    }
    if (args.length() > 6) {
        const auto seventhArg = args.at(6);
        if (seventhArg == "fh")
            flipHorizontally = true;
        else if (seventhArg == "fv")
            flipVertically = true;
    }

    QHash<QString, QImage>::const_iterator it = mImages.constFind(imageFilename);
    if (it == mImages.constEnd()) {
        QImage image(imageFilename);
        if (image.isNull()) {
            qWarning() << "Failed to load image at" << imageFilename;
            return image;
        }

        // TODO: is there a better way of doing this?
        QImage alphaImage = image;
        for (int y = 0; y < alphaImage.height(); ++y) {
            for (int x = 0; x < alphaImage.width(); ++x) {
                const QRgb pixelRgb = alphaImage.pixel(x, y);
                if (pixelRgb == whiteRgba || pixelRgb == magentaRgba)
                    alphaImage.setPixel(x, y, transparentRgba);
            }
        }

        mImages.insert(imageFilename, alphaImage);
        it = mImages.constFind(imageFilename);
        qCDebug(lcRectSpriteImageProvider) << "Added" << imageFilename << "to cache";
    } else {
        qCDebug(lcRectSpriteImageProvider) << "Found" << imageFilename << "in cache";
    }

    // TODO: we should probably cache these sub-images too, as we now draw all of the tiles in the game with them...

    // Copy an individual image out of the larger image, and flip it if necessary.
    QImage rectImage = it.value().copy(QRect(rectX, rectY, rectWidth, rectHeight));
    if (flipHorizontally || flipVertically)
        rectImage = rectImage.mirrored(flipHorizontally, flipVertically);
    *size = rectImage.size();
    return rectImage;
}

int RectSpriteImageProvider::parseInt(const QString &intStr) const
{
    bool convertedToIntSuccessfully = false;
    const int i = intStr.toInt(&convertedToIntSuccessfully);
    if (!convertedToIntSuccessfully) {
        qWarning() << "Failed to convert" << intStr << "to an int";
        return -1;
    }
    return i;
}
