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

PngImage::PngImage(QByteArray pngData)
{
    if (!pngData.isNull()) {
        QBuffer buffer(&pngData);
        setPngData(&buffer);
    }
}

int PngImage::setPngData(QIODevice *buffer)
{
    m_pixmap = QPixmap();
    static const char *pngHeader = "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a";
    static const char *hdrHeader = "IHDR";
    static const char *endHeader = "IEND";
    static const char *aauHeader = "aaUd";
    static const char *endBlock= "\x00\x00\x00\x00""IEND""\xae\x42\x60\x82";
    static const int chunkHeaderSize = 8;
    static const int chunkTagSize = 4;
    qint64 startPos = buffer->pos();
    char header[9];
    header[8] = 0;
    qint64 bytesRead, pos = 0;
    bytesRead = buffer->read(header, chunkHeaderSize);
    if (bytesRead != chunkHeaderSize || qstrcmp(header, pngHeader))
        return 0;
    header[4] = 0;
    int size, skip;
    QDataStream in(buffer);
    in >> size;
    bytesRead = in.readRawData(header, chunkTagSize);
    if (bytesRead != chunkTagSize || qstrcmp(header, hdrHeader))
        return 0;
    pos = buffer->pos();
    skip = size + chunkTagSize;
    while (skip > 0) {
        bytesRead = in.skipRawData(skip); // skip chunk content + crc block
        if (bytesRead != skip)
            return 0;
        pos = buffer->pos();
        skip = 0;
        in >> size;
        bytesRead = in.readRawData(header, chunkTagSize);
        if (bytesRead == chunkTagSize && qstrcmp(header, aauHeader) && qstrcmp(header, endHeader))
            skip = size + chunkTagSize;
    }
    bytesRead = pos - startPos;
    QByteArray pngData;
    pngData.resize(static_cast<int>(bytesRead));
    buffer->seek(startPos);
    buffer->read(pngData.data(), bytesRead);
    pngData.append(endBlock, 12);
    m_pngData = pngData;
    return m_pngData.size();
}

QByteArray PngImage::pngData() const
{
    return m_pngData;
}

QImage PngImage::toQImage() const
{
    if (!m_pngData.isNull() && !m_pngData.isEmpty()) {
        return QImage::fromData(m_pngData, pngFormat);
    }
    return QImage();
}

QPixmap PngImage::toQPixmap()
{
    if (m_pixmap.isNull() && !m_pngData.isNull() && !m_pngData.isEmpty()) {
        QBuffer buffer(&m_pngData);
        buffer.open(QIODevice::ReadOnly);
        QImageReader reader(&buffer, pngFormat);
        m_pixmap = QPixmap::fromImageReader(&reader);
    }
    return m_pixmap;
}
