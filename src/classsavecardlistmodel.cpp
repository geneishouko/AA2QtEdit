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
    CardListModel(parent)
{
    m_classHeaderReader = DataReader::getDataReader("headerdata");
}

ClassSaveCardListModel::~ClassSaveCardListModel()
{
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
    /*for (DataReader::DataBlockList::const_iterator it = m_classData.constBegin(); it != m_classData.constEnd(); it++) {
        m_headerDictionary.insert((*it)->key(), m_classHeaderReader->read(&buffer, (*it)));
    }*/
    m_headerDictionary = m_classHeaderReader->buildDictionary(&buffer, m_classData);
    int studentsCount = m_headerDictionary->value("HEADER_BOYS").toInt() + m_headerDictionary->value("HEADER_GIRLS").toInt();

    // read characters
    int pos = 0;
    int cardOffset, cardEndOffset;
    int cardPlayDataOffset, cardPlayDataEndOffset = 0;
    int playSeat;
    //bool isMale;

    while (studentsCount > 0) {
        cardOffset = data.indexOf(pngHeader, pos);
        if (cardOffset == -1)
            break;
        cardEndOffset = data.indexOf(endBlock, cardOffset + 1);
        cardEndOffset = data.indexOf(endBlock, cardEndOffset + 1) + 12;
        cardPlayDataOffset = cardEndOffset;
        CardFile *card = new CardFile(&buffer, cardOffset, cardEndOffset);
        cardPlayDataEndOffset = card->loadPlayData(&buffer, cardPlayDataOffset);
        addCard(card);

        buffer.seek(cardOffset - 4);
        buffer.read(reinterpret_cast<char*>(&playSeat), 4);
        card->setSeat(playSeat);
        //isMale = data[cardOffset - 5] == '\0';
        pos = cardPlayDataEndOffset;
        studentsCount--;
        // qDebug() << "Card At" << cardOffset << "play data at" << cardPlayDataOffset;
    }

    qint64 footerSize = data.size() - cardPlayDataEndOffset;
    buffer.seek(cardPlayDataEndOffset);
    m_footer.resize(static_cast<int>(footerSize));
    buffer.read(m_footer.data(), footerSize);
    m_filePath = path;
}

bool ClassSaveCardListModel::save()
{
    QSaveFile file(m_filePath);
    file.open(QIODevice::WriteOnly);
    file.write(m_header);
    // qDebug() << "Wrote" << m_header.size() << "header bytes";
    int gender, seat;
    foreach (CardFile *card, cardList()) {
        gender = card->gender();
        seat = card->seat();
        file.write(reinterpret_cast<char*>(&gender), 1);
        file.write(reinterpret_cast<char*>(&seat), 4);
        card->writeToDevice(&file, true);
    }
    file.write(m_footer);
    return file.commit();
}

bool ClassSaveCardListModel::save(const QModelIndex &index)
{
    CardFile *card = index.data(CardFileRole).value<CardFile*>();
    card->commitChanges();
    if (save()) {
        cardSaved(card->modelIndex());
        return true;
    }
    failedSave();
    return false;
}

void ClassSaveCardListModel::saveAll()
{
    CardListModel::commitChanges();
    if (save()) {
        CardList cards = modifiedCardList();
        for (CardList::ConstIterator it = cards.constBegin(); it != cards.constEnd(); it++) {
            cardSaved((*it)->modelIndex());
        }
    }
}

void ClassSaveCardListModel::writeHeader()
{
    QBuffer buffer;
    buffer.setBuffer(&m_header);
    buffer.open(QBuffer::WriteOnly);
    m_classHeaderReader->writeDictionary(&buffer, m_headerDictionary);
}
