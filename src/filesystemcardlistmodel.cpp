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

FileSystemCardListModel::FileSystemCardListModel(const QString &path, QObject *parent) :
    CardListModel(parent)
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
        addCard(cf);
    }
}

FileSystemCardListModel::~FileSystemCardListModel()
{
}

bool FileSystemCardListModel::save()
{
    return true;
}

void FileSystemCardListModel::saveAll()
{
    CardList cards = modifiedCardList();
    for (CardList::ConstIterator it = cards.begin(); it != cards.end(); it++) {
        (*it)->save();
    }
}

