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

CardFile::CardFile() :
    m_face(new PngImage), m_roster(new PngImage)
{
    m_editDataReader = DataReader::getDataReader("chardata");
    m_playDataReader = DataReader::getDataReader("playdata");
    m_editDataDictionary = new Dictionary(this);
    m_editDataModel = new CardDataModel(this);
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
    delete m_face;
    delete m_roster;
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

    const qint32 aauChunkId = 'aaUd';
    const qint32 endChunkId = 'IEND';
    const qint32 versionChunkId = 'Vers';
    const qint32 blobChunkId = 'Blob';

    m_aauHasBlob = false;
    aauDataOffset = -1;
    file->seek(startOffset);
    qint32 chunkLength = 0, chunkId = 0;
    in.skipRawData(8);
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
    m_face->setPngData(file);
    file->seek(cardRosterOffset);
    m_roster->setPngData(file);

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

    m_fullName = QString("%1 %2")
            .arg(m_editDataReader->read(&m_editDataIO, "PROFILE_FAMILY_NAME").toString())
            .arg(m_editDataReader->read(&m_editDataIO, "PROFILE_FIRST_NAME").toString()).trimmed();
}

int CardFile::loadPlayData(QIODevice *file, int offset)
{
    file->seek(offset);
    DataReader::DataBlockList blockList = m_playDataReader->finalizeDataBlocks(file);
    /*for (DataReader::DataBlockList::const_iterator it = blockList.constBegin(); it != blockList.constEnd(); it++) {
        m_editDataDictionary->insert((*it)->key(), m_editDataReader->read(&m_editDataIO, (*it)->key()));
    }*/
    return blockList.last()->nextOffset();
}

CardDataModel* CardFile::getEditDataModel() const
{
    return m_editDataModel;
}

void CardFile::setModelIndex(int index)
{
    m_modelIndex = index;
}

int CardFile::modelIndex() const
{
    return m_modelIndex;
}

QString CardFile::fileName() const
{
    return m_fileName;
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

QVariant CardFile::getAddress(const QString &key) const
{
    return QString("0x%1").arg(m_editDataReader->m_dataBlockMap[key]->offset(), 4, 16, QChar('0'));
}

QVariant CardFile::getType(const QString &key) const
{
    Q_UNUSED(key)
    //return m_editDataReader->m_dataBlockMap[key]->type();
    return "null";
}

QVariant CardFile::getValue(const QString &key) const
{
    return m_editDataDictionary->value(key);
}

QPixmap CardFile::getFace() const
{
    return m_face->toQPixmap();
}

QPixmap CardFile::getRoster() const
{
    return m_roster->toQPixmap();
}

QString CardFile::getFullName() const
{
    return m_fullName;
}

QDateTime CardFile::getModifiedTime() const
{
    return m_modifiedTime;
}

void CardFile::setModifiedTime(const QDateTime &date)
{
    m_modifiedTime = date;
}

void CardFile::setValue(const QString &key, const QVariant &value)
{
    m_editDataDictionary->insert(key, value);
    m_dirtyKeyValues << key;
    emit changed(m_modelIndex);
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
        m_editDataIO.seek(db->offset());
        m_editDataReader->write(&m_editDataIO, db->key(), m_editDataDictionary->value(db->key()));
        it = m_dirtyKeyValues.erase(it);
    }
}

void CardFile::saveChanges()
{
    commitChanges();
    QSaveFile save(m_filePath);
    save.open(QFile::WriteOnly);

    qint64 editOffset, aaudOffset = 0;

    save.write(m_face->pngData());
    if (m_aauDataVersion >= 2) {
        save.seek(save.pos() - 12); // nuke IEND block
        aaudOffset = save.pos();
        save.write(m_aauData);
        save.write("\x00\x00\x00\x00""IEND""\xae\x42\x60\x82", 12);
    }

    editOffset = save.pos();
    qDebug() << "Saving Edit Data at" << editOffset;
    qint64 written = save.write(m_editData);
    Q_ASSERT(written = editDataLength);

    qint32 rosterSize = m_roster->pngData().size();
    save.write(reinterpret_cast<char*>(&rosterSize), 4);
    qDebug() << "Saving roster picture at" << save.pos();
    save.write(m_roster->pngData());

    qint32 finalOffset = static_cast<qint32>(save.pos() - editOffset) + 4;
    qDebug() << "Edit data offset is" << finalOffset;
    save.write(reinterpret_cast<char*>(&finalOffset), 4);

    QFile originalFile(m_filePath);
    originalFile.open(QIODevice::ReadOnly);
    // IThe original file doesn't carry an AAU blob
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

    aaudDelta = static_cast<int>(save.pos() - aaudOffset + footerModcardSize);
    editDelta = static_cast<int>(save.pos() - editOffset + footerModcardSize);

    save.write(footerModcardMagic, 8);
    save.write(reinterpret_cast<char*>(&blobDelta), 4);
    save.write(reinterpret_cast<char*>(&aaudDelta), 4);
    save.write(reinterpret_cast<char*>(&editDelta), 4);

    save.commit();
}
