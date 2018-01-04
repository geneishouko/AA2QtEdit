#ifndef FILESYSTEMCARDLISTMODELLOADER_H
#define FILESYSTEMCARDLISTMODELLOADER_H

#include <QFileInfo>
#include <QThread>

namespace ClassEdit {

    class CardFile;
    class FileSystemCardListModel;

    class FileSystemCardListModelLoader : public QThread
    {
        Q_OBJECT

    public:
        FileSystemCardListModelLoader(FileSystemCardListModel *parent = nullptr);

        void load(const QFileInfo &file);
        virtual void run();

    signals:
        void cardLoaded(CardFile *card);

    private:
        FileSystemCardListModel *m_model;
    };

}

#endif // FILESYSTEMCARDLISTMODELLOADER_H
