#include "filesystemcardlistmodelloader.h"

#include "cardfile.h"
#include "filesystemcardlistmodel.h"

using namespace ClassEdit;

FileSystemCardListModelLoader::FileSystemCardListModelLoader(FileSystemCardListModel *parent):
    QThread(parent),
    m_model(parent)
{
    connect(this, SIGNAL(cardLoaded(CardFile*)), m_model, SLOT(addCard(CardFile*)));
}

void FileSystemCardListModelLoader::load(const QFileInfo &file)
{
    CardFile *card= new CardFile(file);
    if (!card->isValid()) {
        delete card;
        return;
    }
    card->moveToThread(m_model->thread());
    emit cardLoaded(card);
}

void FileSystemCardListModelLoader::run()
{
    QFileInfo file = m_model->takeFile();
    while (file.isFile()) {
        load(file);
        file = m_model->takeFile();
    }
}
