#ifndef PNGIMAGE_H
#define PNGIMAGE_H

#include <QByteArray>
#include <QImage>
#include <QPixmap>

namespace ClassEdit {

    class PngImage
    {
    public:
        PngImage(QByteArray pngData = QByteArray());

        int setPngData(QIODevice *pngData);
        QByteArray pngData() const;
        QImage toQImage() const;
        QPixmap toQPixmap();

    private:
        QByteArray m_pngData;
        QPixmap m_pixmap;
    };

}

#endif // PNGIMAGE_H
