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
    for (DataReader::DataBlockList::const_iterator it = m_classData.constBegin(); it != m_classData.constEnd(); it++) {
        m_headerDictionary->insert((*it)->key(), m_classHeaderReader->read(&buffer, (*it)));
    }
    int studentsCount = m_headerDictionary->value("HEADER_BOYS").toInt() + m_headerDictionary->value("HEADER_GIRLS").toInt();

    // read characters
    int pos = 0;
    int cardOffset, cardEndOffset;
    int cardPlayDataOffset, cardPlayDataEndOffset;
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
        m_cardList << card;

        buffer.seek(cardOffset - 4);
        buffer.read(reinterpret_cast<char*>(&playSeat), 4);
        isMale = data[cardOffset - 5] == '\0';
        pos = cardPlayDataEndOffset;
    }

    qint64 footerSize = buffer.size() - buffer.pos();
    m_footer.resize(static_cast<int>(footerSize));
    buffer.read(m_footer.data(), footerSize);
}

int ClassSaveCardListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_cardList.size();
    return 0;
}

QVariant ClassSaveCardListModel::data(const QModelIndex &index, int role) const
{
    CardFile *card = m_cardList[index.row()];
    if (role == Qt::DisplayRole)
        return card->getFullName();
    else if (role == Qt::DecorationRole)
        return card->getRoster();
    else if (role == CardFileRole) {
        return QVariant::fromValue<CardFile*>(card);
    }
    return QVariant();
}
