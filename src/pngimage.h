#ifndef PNGIMAGE_H
#define PNGIMAGE_H

#include <QByteArray>
#include <QImage>
#include <QPixmap>

namespace ClassEdit {

    namespace PngImage
    {
        QByteArray getPngData(QIODevice *pngData);
        QImage toImage(const QByteArray &data);
        QPixmap toPixmap(const QByteArray &data);
    };

}

#endif // PNGIMAGE_H
