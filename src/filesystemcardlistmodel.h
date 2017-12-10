#ifndef FILESYSTEMCARDLIST_H
#define FILESYSTEMCARDLIST_H

#include <QAbstractListModel>
#include <QFileSystemWatcher>

namespace ClassEdit {

    class CardFile;

    class FileSystemCardListModel : public QAbstractListModel
    {
    public:
        FileSystemCardListModel(const QString &path);
        ~FileSystemCardListModel();

        CardFile *getCard(int index) const;

        int rowCount(const QModelIndex &) const;
        int columnCount(const QModelIndex &) const;
        QVariant data(const QModelIndex &index, int role) const;

    public slots:
        void cardChanged(int index);

    private:
        QList<CardFile*> m_cardList;
        QFileSystemWatcher m_fswatcher;
    };

}

#endif // FILESYSTEMCARDLIST_H
