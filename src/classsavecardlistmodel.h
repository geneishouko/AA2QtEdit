#ifndef CLASSSAVECARDLISTMODEL_H
#define CLASSSAVECARDLISTMODEL_H

#include "datareader.h"
#include "dictionary.h"

#include <QAbstractListModel>
#include <QIODevice>
#include <QSet>

namespace ClassEdit {

    class CardFile;
    class DataReader;
    class Dictionary;

    class ClassSaveCardListModel : public QAbstractListModel
    {
        Q_OBJECT

        QByteArray m_saveHeader;
        QByteArray m_saveFooter;
        QList<QByteArray> m_saveCharacters;

    public:
        explicit ClassSaveCardListModel(QObject *parent = nullptr);
        ~ClassSaveCardListModel();

        void loadFromFile(const QString& path);
        void loadFromIODevice(QIODevice *data);
        CardFile *getCard(int index) const;

        int rowCount(const QModelIndex &parent) const;
        QVariant data(const QModelIndex &index, int role) const;

        bool saveToDisk();
        bool submit();

    public slots:
        void cardChanged(int index);
        void cardSaved(int index);
        void saveAll();

    signals:
        void cardsChanged(int count);

    private:
        QString m_filePath;
        DataReader *m_classHeaderReader;
        Dictionary m_headerDictionary;
        DataReader::DataBlockList m_classData;
        QList<CardFile*> m_cardList;
        QSet<CardFile*> m_changedCardList;

        QByteArray m_header;
        QByteArray m_footer;

    };

}

#endif // CLASSSAVECARDLISTMODEL_H
