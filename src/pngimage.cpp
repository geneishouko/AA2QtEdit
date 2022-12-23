/*
    This file is part of AA2QtEdit.

    AA2QtEdit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AA2QtEdit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AA2QtEdit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pngimage.h"

#include <QBuffer>
#include <QDataStream>
#include <QImageReader>

using namespace ClassEdit;

static const char *pngFormat = "PNG";

QByteArray PngImage::getPngData(QIODevice *buffer)
{
    static const char *pngHeader = "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a";
    static const char *hdrHeader = "IHDR";
    static const char *endHeader = "IEND";
    static const char *aauHeader = "aaUd";
    static const char *phyHeader = "pHYs";
    static const char *endBlock= "\x00\x00\x00\x00""IEND""\xae\x42\x60\x82";
    static const int chunkHeaderSize = 8;
    static const int chunkTagSize = 4;
    QByteArray pngData;
    pngData.reserve(200*1000);

    qint64 startPos = buffer->pos();
    char header[9];
    header[8] = 0;
    qint64 bytesRead, pos = 0;
    bytesRead = buffer->read(header, chunkHeaderSize);
    if (bytesRead != chunkHeaderSize || qstrcmp(header, pngHeader))
        return 0;
    header[4] = 0;
    int size;
    QDataStream in(buffer);
    in >> size;
    bytesRead = in.readRawData(header, chunkTagSize);
    if (bytesRead != chunkTagSize || qstrcmp(header, hdrHeader))
        return 0;

    buffer->seek(startPos);
    size += chunkHeaderSize + chunkTagSize * 3;
    pngData.append(buffer->read(size));

    while (true) {
        pos = buffer->pos();
        in >> size;
        bytesRead = in.readRawData(header, chunkTagSize);
        if (bytesRead == chunkTagSize && qstrcmp(header, aauHeader) && qstrcmp(header, endHeader)) {
            if (qstrcmp(header,phyHeader) == 0) {
                buffer->read(size + chunkTagSize);
                continue;
            }
            buffer->seek(pos);
            QByteArray data = buffer->read(chunkTagSize * 3 + size);
            if (data.size() != chunkTagSize * 3 + size)
                return 0;
            pngData.append(data);
        }
        else {
            break;
        }
    }
    pngData.append(endBlock, 12);
    pngData.squeeze();
    return pngData;
}

QImage PngImage::toImage(const QByteArray &data)
{
    return QImage::fromData(data, pngFormat);
}

QPixmap PngImage::toPixmap(const QByteArray &data)
{
    QBuffer buffer;
    buffer.setData(data);
    buffer.open(QIODevice::ReadOnly);
    QImageReader reader(&buffer, pngFormat);
    return QPixmap::fromImageReader(&reader);
}
