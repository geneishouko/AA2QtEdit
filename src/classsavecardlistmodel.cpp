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

#include "classsavecardlistmodel.h"

#include "cardfile.h"
#include "datareader.h"
#include "dictionary.h"

#include <QBuffer>
#include <QDataStream>
#include <QFile>
#include <QSaveFile>

#include <QtDebug>

using namespace ClassEdit;

ClassSaveCardListModel::ClassSaveCardListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_classHeaderReader = DataReader::getDataReader("headerdata");
    m_headerDictionary = new Dictionary(this);
}

ClassSaveCardListModel::~ClassSaveCardListModel()
{
    foreach(CardFile *card, m_cardList)
        delete card;
    foreach(DataBlock *db, m_classData)
        delete db;
}

void ClassSaveCardListModel::loadFromFile(const QString &path)
{
    static const char *pngHeader = "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a";
    static const char *endBlock= "IEND";
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    m_classData = m_classHeaderReader->finalizeDataBlocks(&buffer);
    int headerSize = m_classData.last()->nextOffset();
    file.seek(0);
    m_header.resize(headerSize);
    file.read(m_header.data(), headerSize);
    for (DataReader::DataBlockList::const_iterator it = m_classData.constBegin(); it != m_classData.constEnd(); it++) {
        m_headerDictionary->insert((*it)->key(), m_classHeaderReader->read(&buffer, (*it)));
    }
    int studentsCount = m_headerDictionary->value("HEADER_BOYS").toInt() + m_headerDictionary->value("HEADER_GIRLS").toInt();

    // read characters
    int pos = 0;
    int cardOffset, cardEndOffset;
    int cardPlayDataOffset, cardPlayDataEndOffset = 0;
    int playSeat;
    bool isMale;

    while (studentsCount > 0) {
        cardOffset = data.indexOf(pngHeader, pos);
        if (cardOffset == -1)
            break;
        cardEndOffset = data.indexOf(endBlock, cardOffset + 1);
        cardEndOffset = data.indexOf(endBlock, cardEndOffset + 1) + 12;
        cardPlayDataOffset = cardEndOffset;
        CardFile *card = new CardFile(&buffer, cardOffset, cardEndOffset);
        cardPlayDataEndOffset = card->loadPlayData(&buffer, cardPlayDataOffset);
        card->setParentModel(this);
        card->setModelIndex(m_cardList.size());
        QObject::connect(card, &CardFile::changed, this, &ClassSaveCardListModel::cardChanged);
        m_cardList << card;

        buffer.seek(cardOffset - 4);
        buffer.read(reinterpret_cast<char*>(&playSeat), 4);
        card->setSeat(playSeat);
        isMale = data[cardOffset - 5] == '\0';
        pos = cardPlayDataEndOffset;
        qDebug() << "Card At" << cardOffset << "play data at" << cardPlayDataOffset;
    }

    qint64 footerSize = data.size() - cardPlayDataEndOffset;
    buffer.seek(cardPlayDataEndOffset);
    m_footer.resize(static_cast<int>(footerSize));
    buffer.read(m_footer.data(), footerSize);
    m_filePath = path;
}

int ClassSaveCardListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_cardList.size();
}

QVariant ClassSaveCardListModel::data(const QModelIndex &index, int role) const
{
    CardFile *card = m_cardList[index.row()];
    if (role == Qt::DisplayRole)
        return card->fullName();
    else if (role == Qt::DecorationRole)
        return card->getRoster();
    else if (role == CardFileRole) {
        return QVariant::fromValue<CardFile*>(card);
    }
    return QVariant();
}

bool ClassSaveCardListModel::saveToDisk()
{
    QSaveFile file(m_filePath);
    file.open(QIODevice::WriteOnly);
    file.write(m_header);
    qDebug() << "Wrote"<<m_header.size()<<"header bytes";
    int gender, seat;
    foreach (CardFile *card, m_cardList) {
        gender = card->getGender();
        seat = card->seat();
        file.write(reinterpret_cast<char*>(&gender), 1);
        file.write(reinterpret_cast<char*>(&seat), 4);
        card->writeToDevice(&file, true);
    }
    file.write(m_footer);
    return file.commit();
}

bool ClassSaveCardListModel::submit()
{
    return saveToDisk();
}

void ClassSaveCardListModel::cardChanged(int cardIndex)
{
    const QModelIndex &modelIndex = index(cardIndex);
    emit dataChanged(modelIndex, modelIndex);
}
