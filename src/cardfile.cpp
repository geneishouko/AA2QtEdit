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
#include "datareader.h"
#include "dictionary.h"
#include "pngimage.h"

#include <QBuffer>
#include <QFile>
#include <QImageReader>
#include <QSaveFile>

#include <QtDebug>

using namespace ClassEdit;

static const qint32 editDataLength = 3011;
static const QString facePngKey("FACE_PNG_DATA");
static const QString rosterPngKey("ROSTER_PNG_DATA");

CardFile::CardFile()
{
    m_editDataReader = DataReader::getDataReader("chardata");
    m_playDataReader = DataReader::getDataReader("playdata");
    m_editDataDictionary = new Dictionary(this);
    m_editDataModel = new CardDataModel(this);
    m_parentModel = nullptr;
    m_seat = 0;
    m_isValid = true;
}

CardFile::CardFile(const QFileInfo &file) :
    CardFile()
{
    m_filePath = file.absoluteFilePath();
    m_fileName = file.fileName();
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

void CardFile::setParentModel(QAbstractItemModel *model)
{
    m_parentModel = model;
}

void CardFile::init(QIODevice *file, qint64 startOffset, qint64 endOffset)
{
    qint32 cardRosterOffset;
    qint32 cardRosterLength;
    qint32 editDataOffset;
    qint32 aauDataOffset;
    qint32 m_aauDataLength = 0;
    qint32 aauBlobOffset;
    qint32 aauBlobLength;
    bool m_aauHasBlob;

    QDataStream in(file);
    in.setByteOrder(QDataStream::LittleEndian);

    file->seek(endOffset - 4);
    in >> editDataOffset;
    editDataOffset = static_cast<qint32>(endOffset) - editDataOffset;

    cardRosterOffset = editDataOffset + editDataLength + 4;
    file->seek(cardRosterOffset - 4);
    in >> cardRosterLength;
    cardRosterLength -= 4;

    //m_editDataOffset = m_cardRosterOffset + m_cardRosterLength;
    //m_editDataLength = rawLength - m_editDataOffset - 4;

    static const char *pngHeader = "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a";
    static const qint32 aauChunkId = 'aaUd';
    static const qint32 endChunkId = 'IEND';
    static const qint32 versionChunkId = 'Vers';
    static const qint32 blobChunkId = 'Blob';

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

    m_aauHasBlob = false;
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
        //Q_ASSERT(chunkId == versionChunkId);
        //Q_ASSERT(chunkLength == 3);
        if (chunkId == versionChunkId) {
            m_aauDataVersion = chunkLength;
            in >> chunkId;
            if (chunkId == blobChunkId) {
                m_aauHasBlob = true;
                aauBlobOffset = -1;
                file->seek(endOffset - 20);
                QByteArray magic(8, 0);
                in.readRawData(magic.data(), 8);
                if (magic == "MODCARD3") {
                    in >> aauBlobOffset;
                    aauBlobOffset = endOffset - aauBlobOffset;
                }
            }
        }
    }

    file->seek(startOffset);
    setEditDataValue(facePngKey, PngImage::getPngData(file));
    file->seek(cardRosterOffset);
    setEditDataValue(rosterPngKey, PngImage::getPngData(file));

    if (m_aauDataVersion >= 2) {
        m_aauData.resize(m_aauDataLength);
        file->seek(aauDataOffset);
        file->read(m_aauData.data(), m_aauDataLength);
    }

    m_editData.resize(editDataLength);
    file->seek(editDataOffset);
    file->read(m_editData.data(), editDataLength);
    m_editDataIO.setBuffer(&m_editData);
    m_editDataIO.open(QIODevice::ReadWrite);

    DataReader::DataBlockList &blockList = m_editDataReader->m_dataBlocks;
    for (DataReader::DataBlockList::const_iterator it = blockList.constBegin(); it != blockList.constEnd(); it++) {
        m_editDataDictionary->insert((*it)->key(), m_editDataReader->read(&m_editDataIO, (*it)->key()));
    }

    updateQuickInfoGetters();
    m_dirtyKeyValues.clear();
}

int CardFile::loadPlayData(QIODevice *file, int offset)
{
    file->seek(offset);
    DataReader::DataBlockList blockList = m_playDataReader->finalizeDataBlocks(file);
    /*for (DataReader::DataBlockList::const_iterator it = blockList.constBegin(); it != blockList.constEnd(); it++) {
        m_editDataDictionary->insert((*it)->key(), m_editDataReader->read(&m_editDataIO, (*it)->key()));
    }*/
    qint64 playDataEnd = blockList.last()->nextOffset();
    qint64 playDataSize = playDataEnd - offset;
    m_playData.resize(static_cast<int>(playDataSize));
    file->seek(offset);
    file->read(m_playData.data(), playDataSize);
    return static_cast<int>(playDataEnd);
}

bool CardFile::isValid() const
{
    return m_isValid;
}

CardDataModel *CardFile::getEditDataModel() const
{
    return m_editDataModel;
}

QString CardFile::fileName() const
{
    return m_fileName;
}

QString CardFile::filePath() const
{
    return m_filePath;
}

QVector<QString> CardFile::getEditDataKeys() const
{
    QVector<QString> keyList;
    DataReader::DataBlockList &blockList = m_editDataReader->m_dataBlocks;
    keyList.reserve(blockList.size());
    for (DataReader::DataBlockList::const_iterator it = blockList.constBegin(); it != blockList.constEnd(); it++) {
        keyList.push_back((*it)->key());
    }
    return keyList;
}

QVariant CardFile::getEditDataAddress(const QString &key) const
{
    return QString("0x%1").arg(m_editDataReader->m_dataBlockMap[key]->offset(), 4, 16, QChar('0'));
}

QVariant CardFile::getEditDataType(const QString &key) const
{
    Q_UNUSED(key)
    //return m_editDataReader->m_dataBlockMap[key]->type();
    return "null";
}

QVariant CardFile::getEditDataValue(const QString &key) const
{
    return m_editDataDictionary->value(key);
}

int CardFile::getGender() const
{
    return m_gender;
}

QPixmap CardFile::getFace()
{
    if (m_face.isNull())
        m_face = PngImage::toPixmap(getEditDataValue(facePngKey).toByteArray());
    return m_face;
}

QPixmap CardFile::getRoster()
{
    if (m_roster.isNull())
        m_roster= PngImage::toPixmap(getEditDataValue(rosterPngKey).toByteArray());
    return m_roster;
}

QString CardFile::fullName() const
{
    return m_fullName;
}

QDateTime CardFile::modifiedTime() const
{
    return m_modifiedTime;
}

int CardFile::seat() const
{
    return m_seat;
}

void CardFile::setModifiedTime(const QDateTime &date)
{
    m_modifiedTime = date;
}

void CardFile::setSeat(int seat)
{
    m_seat = seat;
}

void CardFile::setFace(QIODevice *file)
{
    m_face = QPixmap();
    setEditDataValue(facePngKey, PngImage::getPngData(file));
}

void CardFile::setRoster(QIODevice *file)
{
    m_roster = QPixmap();
    setEditDataValue(rosterPngKey, PngImage::getPngData(file));
}

void CardFile::updateQuickInfoGetters()
{
    m_gender = m_editDataReader->read(&m_editDataIO, "PROFILE_GENDER").toInt();
    m_fullName = QString("%1 %2")
            .arg(m_editDataReader->read(&m_editDataIO, "PROFILE_FAMILY_NAME").toString())
            .arg(m_editDataReader->read(&m_editDataIO, "PROFILE_FIRST_NAME").toString()).trimmed();
}

void CardFile::setEditDataValue(const QString &key, const QVariant &value)
{
    m_editDataDictionary->insert(key, value);
    m_dirtyKeyValues << key;
    emit changed(m_modelIndex);
}

void CardFile::setModelIndex(int index)
{
    m_modelIndex = index;
}

bool CardFile::hasPendingChanges() const
{
    return !m_dirtyKeyValues.empty();
}

void CardFile::commitChanges()
{
    DataBlock *db;
    for(QSet<QString>::ConstIterator it = m_dirtyKeyValues.cbegin(); it != m_dirtyKeyValues.cend();) {
        db = m_editDataReader->m_dataBlockMap[*it];
        if (!db) {
            it = m_dirtyKeyValues.erase(it);
            continue;
        }
        m_editDataIO.seek(db->offset());
        m_editDataReader->write(&m_editDataIO, db->key(), m_editDataDictionary->value(db->key()));
        it = m_dirtyKeyValues.erase(it);
    }
}

void CardFile::saveToFile(const QString &file)
{
    QSaveFile save(file);
    save.open(QFile::WriteOnly);
    qint64 editOffset, aaudOffset;
    writeToDevice(&save, &editOffset, &aaudOffset);

    QFile originalFile(m_filePath);
    originalFile.open(QIODevice::ReadOnly);
    // Appended AAU blobs only from AAU data ver 3
    if (m_aauDataVersion < 3) {
        save.commit();
        return;
    }

    int blobDelta, aaudDelta, editDelta;
    static const int footerModcardSize = 8 + 4 * 3;
    static const char *footerModcardMagic = "MODCARD3";
    char magic[8];
    static const int tempSize = 1024 * 1024;

    originalFile.seek(originalFile.size() - footerModcardSize);
    originalFile.read(magic, 8);
    if (qstrcmp(magic, footerModcardMagic)) {
        save.commit();
        return;
    }

    originalFile.read(reinterpret_cast<char*>(&blobDelta), 4);

    qint64 blobSize = blobDelta - footerModcardSize;
    qint64 bytesWritten = 0;
    QByteArray temp;
    temp.resize(tempSize);

    originalFile.seek(originalFile.size() - blobDelta);
    bytesWritten = originalFile.read(temp.data(), tempSize > blobSize ? blobSize : tempSize);
    while (bytesWritten > 0 && blobSize > 0) {
        blobSize -= bytesWritten;
        save.write(temp.data(), bytesWritten);
        bytesWritten = originalFile.read(temp.data(), tempSize < blobSize ? tempSize : blobSize);
        if (bytesWritten < 1)
            save.cancelWriting();
    }
    originalFile.close();

    aaudDelta = static_cast<int>(save.pos() - aaudOffset + footerModcardSize);
    editDelta = static_cast<int>(save.pos() - editOffset + footerModcardSize);

    save.write(footerModcardMagic, 8);
    save.write(reinterpret_cast<char*>(&blobDelta), 4);
    save.write(reinterpret_cast<char*>(&aaudDelta), 4);
    save.write(reinterpret_cast<char*>(&editDelta), 4);

    save.commit();
}

void CardFile::save()
{
    commitChanges();
    if (m_parentModel && m_filePath.isEmpty()) // If this CardFile was read from a class save
        m_parentModel->submit();
    else
        saveToFile(m_filePath);
}

void CardFile::writeToDevice(QIODevice *device, qint64 *editOffset, qint64 *aaudOffset)
{
    qDebug() << "Writing card at" << device->pos();
    qint64 locEditOffset;
    device->write(getEditDataValue(facePngKey).toByteArray());
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
    Q_ASSERT(written = editDataLength);

    QByteArray rosterPng = getEditDataValue(rosterPngKey).toByteArray();
    qint32 rosterSize = rosterPng.size();
    device->write(reinterpret_cast<char*>(&rosterSize), 4);
    device->write(rosterPng);

    qint32 finalOffset = static_cast<qint32>(device->pos() - locEditOffset) + 4;
    device->write(reinterpret_cast<char*>(&finalOffset), 4);
    if (!m_playData.isEmpty()) {
        qDebug() << "Play data at" << device->pos();
        device->write(m_playData);
    }
}
