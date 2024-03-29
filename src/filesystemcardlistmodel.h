#ifndef FILESYSTEMCARDLIST_H
#define FILESYSTEMCARDLIST_H

#include "cardlistmodel.h"

#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QMutex>
#include <QSet>

namespace ClassEdit {

    class CardFile;
    class FileSystemCardListModelLoader;

    class FileSystemCardListModel : public CardListModel
    {
        Q_OBJECT

    public:
        FileSystemCardListModel(const QStringList &fileList, QObject *parent = nullptr);
        FileSystemCardListModel(const QString &path, QObject *parent = nullptr);
        ~FileSystemCardListModel() override;

        void loadDirectory(const QString &path);
        void loadCards();
        bool save() override;
        bool save(const QModelIndex &) override;
        void saveAll() override;
        QFileInfo takeFile();

    public slots:
        void finished();

    private:
        QSet<QObject*> m_threadPool;
        QFileSystemWatcher m_fswatcher;
        QFileInfoList m_loadFileQueue;
        mutable QMutex m_mutex;
    };

}

#endif // FILESYSTEMCARDLIST_H
