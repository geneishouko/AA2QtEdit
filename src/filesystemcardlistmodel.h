#ifndef FILESYSTEMCARDLIST_H
#define FILESYSTEMCARDLIST_H

#include <QAbstractListModel>
#include <QFileSystemWatcher>
#include <QSet>

namespace ClassEdit {

    class CardFile;

    class FileSystemCardListModel : public QAbstractListModel
    {
        Q_OBJECT

    public:
        FileSystemCardListModel(const QString &path);
        ~FileSystemCardListModel();

        CardFile *getCard(int index) const;

        int rowCount(const QModelIndex &) const;
        int columnCount(const QModelIndex &) const;
        QVariant data(const QModelIndex &index, int role) const;

    public slots:
        void cardChanged(int index);
        void cardSaved(int index);
        void saveAll();

    signals:
        void cardsChanged(int count);

    private:
        QList<CardFile*> m_cardList;
        QSet<CardFile*> m_changedCardList;
        QFileSystemWatcher m_fswatcher;
    };

}

#endif // FILESYSTEMCARDLIST_H
