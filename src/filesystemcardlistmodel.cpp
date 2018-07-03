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
#include "filesystemcardlistmodelloader.h"

#include <QDir>
#include <QMutexLocker>

using namespace ClassEdit;

FileSystemCardListModel::FileSystemCardListModel(const QString &path, QObject *parent) :
    CardListModel(parent)
{
    m_fswatcher.addPath(path);
    QDir dir(path);
    QStringList filter;
    filter << "*.png";
    m_loadFileQueue = dir.entryInfoList(filter, QDir::Files, QDir::Time);
    int maxThreads = QThread::idealThreadCount();
    if (maxThreads > 4)
        maxThreads = 4;
    for (int i = 0; i < maxThreads; i++) {
        FileSystemCardListModelLoader *thread = new FileSystemCardListModelLoader(this);
        connect(thread, &QThread::finished, this, &FileSystemCardListModel::finished);
        m_threadPool << qobject_cast<QObject*>(thread);
        thread->start();
    }
}

FileSystemCardListModel::~FileSystemCardListModel()
{
}

void FileSystemCardListModel::finished()
{
    QObject *s = sender();
    s->deleteLater();
    m_threadPool.remove(s);
    if (m_threadPool.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, cardList().size() - 1);
        endInsertRows();
        if (FileSystemCardListModelLoader::error()) {
            emit(notify(tr("Not enough system memory to load all cards!"), 0));
        }
    }
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

QFileInfo FileSystemCardListModel::takeFile()
{
    QMutexLocker locker(&m_mutex);
    if (!m_loadFileQueue.isEmpty())
        return m_loadFileQueue.takeLast();
    return QFileInfo();
}
