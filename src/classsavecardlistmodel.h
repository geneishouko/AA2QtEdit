#ifndef CLASSSAVECARDLISTMODEL_H
#define CLASSSAVECARDLISTMODEL_H

#include "cardlistmodel.h"
#include "datareader.h"
#include "dictionary.h"

namespace ClassEdit {

    class CardFile;
    class DataReader;
    class Dictionary;

    class ClassSaveCardListModel : public CardListModel
    {
        Q_OBJECT

        QByteArray m_saveHeader;
        QByteArray m_saveFooter;

    public:
        explicit ClassSaveCardListModel(QObject *parent = nullptr);
        ~ClassSaveCardListModel();

        inline QByteArray &footer() {
            return m_footer;
        }
        inline Dictionary *headerDictionary() {
            return m_headerDictionary;
        }
        bool loadFromFile(const QString& path);
        bool save();
        bool save(const QModelIndex &);
        void saveAll();
        void writeHeader();

    private:
        QString m_filePath;
        DataReader *m_classHeaderReader;
        Dictionary *m_headerDictionary;
        DataReader::DataBlockList m_classData;

        QByteArray m_header;
        QByteArray m_footer;
    };

}

#endif // CLASSSAVECARDLISTMODEL_H
