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

        inline static bool error() {
            return m_badalloc;
        }
        void load(const QFileInfo &file);
        static void reset() {
            m_badalloc = false;
        }
        virtual void run();

    signals:
        void cardLoaded(CardFile *card);

    private:
        FileSystemCardListModel *m_model;
        static bool m_badalloc;
    };

}

#endif // FILESYSTEMCARDLISTMODELLOADER_H
