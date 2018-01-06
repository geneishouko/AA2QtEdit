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

#include "cardfile.h"

#include "carddatamodel.h"
#include "clothdata.h"
#include "constants.h"
#include "datareader.h"
#include "pngimage.h"

#include <QBuffer>
#include <QFile>
#include <QImageReader>
#include <QSaveFile>

#include <QtDebug>

using namespace ClassEdit;


CardFile::CardFile()
{
    m_editDataReader = DataReader::getDataReader("chardata");
    m_playDataReader = DataReader::getDataReader("playdata");
    m_editDataModel = nullptr;
    m_playDataModel = nullptr;
    m_playDataDictionary = nullptr;
    m_seat = 0;
    m_isValid = true;
}

CardFile::CardFile(const QFileInfo &file) :
    CardFile()
{
    m_filePath = file.absoluteFilePath();
    m_fileName = file.fileName();
    m_modifiedTime = file.lastModified();
    QFile io;
    io.setFileName(m_filePath);
    io.open(QFile::ReadOnly);
    init(&io, 0, io.size());
}

CardFile::CardFile(QIODevice *file, qint64 startOffset, qint64 endOffset) :
    CardFile()
{
    init(file, startOffset, endOffset);
}

CardFile::~CardFile()
{
}

void CardFile::init(QIODevice *file, qint64 startOffset, qint64 endOffset)
{
    qint32 cardRosterOffset;
    qint32 cardRosterLength;
    qint32 editDataOffset;
    qint32 aauDataOffset;
    qint32 m_aauDataLength = 0;

    QDataStream in(file);
    in.setByteOrder(QDataStream::LittleEndian);

    file->seek(endOffset - 4);
    in >> editDataOffset;
    editDataOffset = static_cast<qint32>(endOffset) - editDataOffset;

    cardRosterOffset = editDataOffset + EditDataLength + 4;
    file->seek(cardRosterOffset - 4);
    in >> cardRosterLength;
    cardRosterLength -= 4;

    //m_editDataOffset = m_cardRosterOffset + m_cardRosterLength;
    //m_editDataLength = rawLength - m_editDataOffset - 4;

    static const char *pngHeader = "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a";
    static const qint32 aauChunkId = 'aaUd';
    static const qint32 endChunkId = 'IEND';
    static const qint32 versionChunkId = 'Vers';

    static const char editHeader[] = "\x81\x79\x83\x47\x83\x66\x83\x42\x83\x62\x83\x67\x81\x7a";
    if (!file->seek(editDataOffset)) {
        m_isValid = false;
        return;
    }
    m_editData = file->read(sizeof(editHeader) - 1);
    if (m_editData != editHeader) {
        m_isValid = false;
        return;
    }
    m_editData.resize(0);

    aauDataOffset = -1;
    file->seek(startOffset);
    qint32 chunkLength = 0, chunkId = 0;
    m_editData = file->read(8);
    if (m_editData != pngHeader) {
        m_isValid = false;
        return;
    }
    m_editData.resize(0);
    in.setByteOrder(QDataStream::BigEndian);
    while(!file->atEnd()) {
        in >> chunkLength;
        in >> chunkId;
        if (chunkId == aauChunkId) {
            aauDataOffset = static_cast<qint32>(file->pos()) - 8;
            m_aauDataLength = chunkLength + 12;
            break;
        }
        else if (chunkId == endChunkId) {
            break;
        }
        in.skipRawData(chunkLength + 4); //skip data + CRC blocks
    }
    m_aauDataVersion = -1;
    if (aauDataOffset > 0) {
        in.setByteOrder(QDataStream::LittleEndian);
        file->seek(aauDataOffset + 8); // skip AAU header
        in >> chunkId;
        in >> chunkLength;
        if (chunkId == versionChunkId) {
            m_aauDataVersion = chunkLength;
        }
    }

    m_editData.resize(EditDataLength);
    file->seek(editDataOffset);
    file->read(m_editData.data(), EditDataLength);
    m_editDataIO.setBuffer(&m_editData);
    m_editDataIO.open(QIODevice::ReadWrite);

    m_editDataDictionary = m_editDataReader->buildDictionary(&m_editDataIO, m_editDataReader->m_dataBlocks);
    m_editDataDictionary->setParent(this);
    updateQuickInfoGetters();
    connect(m_editDataDictionary, &Dictionary::changed, this, &CardFile::dictionaryChanged);

    file->seek(startOffset);
    m_editDataDictionary->insert(PortraitPngKey, PngImage::getPngData(file));
    file->seek(cardRosterOffset);
    m_editDataDictionary->insert(ThumbnailPngKey, PngImage::getPngData(file));

    if (m_aauDataVersion >= 2) {
        m_aauData.resize(m_aauDataLength);
        file->seek(aauDataOffset);
        file->read(m_aauData.data(), m_aauDataLength);
    }

    m_editDataModel = new CardDataModel(m_editDataDictionary);
    emit changed(m_modelIndex);
}

int CardFile::loadPlayData(QIODevice *file, int offset)
{
    file->seek(offset);
    DataReader::DataBlockList blockList = m_playDataReader->finalizeDataBlocks(file);
    qint64 playDataEnd = blockList.last()->nextOffset();
    qint64 playDataSize = playDataEnd - offset;
    m_playData.resize(static_cast<int>(playDataSize));
    file->seek(offset);
    file->read(m_playData.data(), playDataSize);

    for (DataReader::DataBlockList::const_iterator it = blockList.constBegin(); it != blockList.constEnd(); it++) {
        m_playDataReader->finalizeDataBlockOffset(*it, offset);
    }
    m_playDataIO.setBuffer(&m_playData);
    m_playDataIO.open(QIODevice::ReadWrite);
    m_playDataDictionary = m_playDataReader->buildDictionary(&m_playDataIO, blockList);
    m_playDataDictionary->setDeleteDataBlocksOnDestruction(true);
    m_playDataDictionary->setParent(this);
    m_playDataModel = new CardDataModel(m_playDataDictionary);
    return static_cast<int>(playDataEnd);
}

const QPixmap &CardFile::portraitPixmap()
{
    if (m_portrait.isNull())
        m_portrait = PngImage::toPixmap(portraitData());
    return m_portrait;
}

const QPixmap &CardFile::thumbnailPixmap()
{
    if (m_thumbnail.isNull())
        m_thumbnail= PngImage::toPixmap(thumbnailData());
    return m_thumbnail;
}

void CardFile::setClothes(const QString &slot, ClothData *cloth)
{
    QVariantMap values = cloth->getValues(slot);
    for (QVariantMap::ConstIterator it = values.cbegin(); it != values.cend(); it++) {
        m_editDataDictionary->set(it.key(), it.value());
    }
}

void CardFile::setPortrait(const QByteArray &file)
{
    QBuffer buffer;
    buffer.setData(file);
    buffer.open(QIODevice::ReadOnly);
    setPortrait(&buffer);
}

void CardFile::setPortrait(QIODevice *file)
{
    m_portrait = QPixmap();
    m_editDataDictionary->set(ThumbnailPngKey, PngImage::getPngData(file));
}

void CardFile::setThumbnail(const QByteArray &file)
{
    QBuffer buffer;
    buffer.setData(file);
    buffer.open(QIODevice::ReadOnly);
    setThumbnail(&buffer);
}

void CardFile::setThumbnail(QIODevice *file)
{
    m_thumbnail = QPixmap();
    m_editDataDictionary->set(PortraitPngKey, PngImage::getPngData(file));
}

void CardFile::updateQuickInfoGetters()
{
    m_gender = m_editDataDictionary->value("PROFILE_GENDER").toInt();
    m_fullName = QString("%1 %2")
            .arg(m_editDataDictionary->value("PROFILE_FAMILY_NAME").toString())
            .arg(m_editDataDictionary->value("PROFILE_FIRST_NAME").toString()).trimmed();
}

void CardFile::commitChanges()
{
    DataBlock *db;
    for(Dictionary::IndexSet::Iterator it = m_editDataDictionary->dirtyIndexBegin(); it != m_editDataDictionary->dirtyIndexEnd(); it++) {
        if (*it >= m_editDataReader->m_dataBlocks.count())
            continue;
        db = m_editDataReader->m_dataBlocks[*it];
        if (!db) {
            continue;
        }
        m_editDataIO.seek(db->offset());
        m_editDataReader->write(&m_editDataIO, db->key(), m_editDataDictionary->value(db->key()));
    }
    m_editDataDictionary->resetDirtyValues();
    m_playDataIO.seek(0);
    m_playDataReader->writeDictionary(&m_playDataIO, m_playDataDictionary);
}

void CardFile::saveToFile(const QString &file)
{
    QSaveFile save(file);
    save.open(QFile::WriteOnly);
    qint64 editOffset, aaudOffset;
    writeToDevice(&save, false, &editOffset, &aaudOffset);

    // Appended AAU blobs only from AAU data ver 3
    if (m_aauDataVersion < 3) {
        save.commit();
        return;
    }

    int blobDelta, aaudDelta, editDelta;
    static const int footerModcardSize = 8 + 4 * 3;
    static const char *footerModcardMagic = "MODCARD3";
    char magic[9];
    magic[8] = 0;
    static const int tempSize = 1024 * 1024;

    QFile originalFile(m_filePath);
    originalFile.open(QIODevice::ReadOnly);
    originalFile.seek(originalFile.size() - footerModcardSize);
    originalFile.read(magic, 8);
    if (qstrcmp(magic, footerModcardMagic)) {
        originalFile.close();
        save.commit();
        return;
    }

    originalFile.read(reinterpret_cast<char*>(&blobDelta), 4);

    qint64 blobSize = blobDelta - footerModcardSize;
    qint64 bytesWritten = 0;
    QByteArray temp;
    temp.resize(tempSize);

    originalFile.seek(originalFile.size() - blobDelta);
    do {
        bytesWritten = originalFile.read(temp.data(), tempSize < blobSize ? tempSize : blobSize);
        save.write(temp.data(), bytesWritten);
        blobSize -= bytesWritten;
    } while (bytesWritten > 0 && blobSize > 0);
    if (blobSize > 0)
        save.cancelWriting();
    originalFile.close();

    // Skip aaUd png header
    aaudDelta = static_cast<int>(save.pos() - aaudOffset - 8 + footerModcardSize);
    editDelta = static_cast<int>(save.pos() - editOffset + footerModcardSize);

    save.write(footerModcardMagic, 8);
    save.write(reinterpret_cast<char*>(&blobDelta), 4);
    save.write(reinterpret_cast<char*>(&aaudDelta), 4);
    save.write(reinterpret_cast<char*>(&editDelta), 4);

    qDebug() << "Saved modcard" << save.commit();
}

void CardFile::save()
{
    commitChanges();
    updateQuickInfoGetters();
    if (m_filePath.isEmpty()) // If this CardFile was read from a class save
        emit(saveRequest(m_modelIndex));
    else
        saveToFile(m_filePath);
    emit changed(m_modelIndex);
    emit saved(m_modelIndex);
}

void CardFile::writeToDevice(QIODevice *device, bool writePlayData, qint64 *editOffset, qint64 *aaudOffset) const
{
    qint64 locEditOffset;
    device->write(m_editDataDictionary->value(PortraitPngKey).toByteArray());
    if (m_aauDataVersion >= 2) {
        device->seek(device->pos() - 12); // nuke IEND block
        if (aaudOffset)
            *aaudOffset = device->pos();
        device->write(m_aauData);
        device->write("\x00\x00\x00\x00""IEND""\xae\x42\x60\x82", 12);
    }

    locEditOffset = device->pos();
    if (editOffset)
        *editOffset = locEditOffset;
    qint64 written = device->write(m_editData);
    Q_ASSERT(written == EditDataLength);

    QByteArray rosterPng = m_editDataDictionary->value(ThumbnailPngKey).toByteArray();
    qint32 rosterSize = rosterPng.size();
    device->write(reinterpret_cast<char*>(&rosterSize), 4);
    device->write(rosterPng);

    qint32 finalOffset = static_cast<qint32>(device->pos() - locEditOffset) + 4;
    device->write(reinterpret_cast<char*>(&finalOffset), 4);
    if (writePlayData && !m_playData.isEmpty()) {
        device->write(m_playData);
    }
}

void CardFile::dictionaryChanged()
{
    emit changed(m_modelIndex);
}
