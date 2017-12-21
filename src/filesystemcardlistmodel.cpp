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

#include "filesystemcardlistmodel.h"

#include "cardfile.h"

#include <QDir>

using namespace ClassEdit;

FileSystemCardListModel::FileSystemCardListModel(const QString &path)
{
    m_fswatcher.addPath(path);
    QDir dir(path);
    QStringList filter;
    filter << "*.png";
    QFileInfoList files = dir.entryInfoList(filter, QDir::Files, QDir::Time);

    CardFile *cf;
    foreach (const QFileInfo &file, files) {
        cf = new CardFile(file);
        if (!cf->isValid()) {
            delete cf;
            continue;
        }
        cf->setParent(this);
        cf->setModifiedTime(file.lastModified());
        cf->setModelIndex(m_cardList.size());
        QObject::connect(cf, &CardFile::changed, this, &FileSystemCardListModel::cardChanged);
        QObject::connect(cf, &CardFile::saved, this, &FileSystemCardListModel::cardSaved);
        m_cardList << cf;
    }
}

FileSystemCardListModel::~FileSystemCardListModel()
{
    foreach(CardFile *card, m_cardList)
        delete card;
}

CardFile *FileSystemCardListModel::getCard(int index) const
{
    return m_cardList[index];
}

int FileSystemCardListModel::rowCount(const QModelIndex &) const
{
    return m_cardList.size();
}

int FileSystemCardListModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant FileSystemCardListModel::data(const QModelIndex &index, int role) const
{
    CardFile *card = m_cardList[index.row()];
    if (role == Qt::DisplayRole)
        return card->fullName();
    else if (role == Qt::DecorationRole)
        return card->getRoster();
    else if (role == CardFileRole) {
        return QVariant::fromValue<CardFile*>(card);
    }
    else if (role == CardModifiedTimeRole) {
        return card->modifiedTime();
    }
    return QVariant();
}

void FileSystemCardListModel::cardChanged(int cardIndex)
{
    m_changedCardList << m_cardList[cardIndex];
    emit cardsChanged(m_changedCardList.size());
    const QModelIndex &modelIndex = index(cardIndex);
    emit dataChanged(modelIndex, modelIndex);
}

void FileSystemCardListModel::cardSaved(int index)
{
    m_changedCardList.remove(m_cardList[index]);
    emit cardsChanged(m_changedCardList.size());
}

void FileSystemCardListModel::saveAll()
{
    // CardFiles will emit saved, which will cause for this model to remove them from this set
    // don't iterate, just check for empty set
    while(!m_changedCardList.empty()) {
        (*m_changedCardList.cbegin())->save();
    }
    emit cardsChanged(m_changedCardList.size());
}

