#ifndef FILESYSTEMCARDLIST_H
#define FILESYSTEMCARDLIST_H

#include "cardlistmodel.h"

#include <QFileSystemWatcher>

namespace ClassEdit {

    class CardFile;

    class FileSystemCardListModel : public CardListModel
    {
        Q_OBJECT

    public:
        FileSystemCardListModel(const QString &path, QObject *parent = nullptr);
        ~FileSystemCardListModel();

        bool save();
        void saveAll();

    private:
        QFileSystemWatcher m_fswatcher;
    };

}

#endif // FILESYSTEMCARDLIST_H
