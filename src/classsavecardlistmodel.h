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

        void loadFromFile(const QString& path);
        bool save();
        void saveAll();

    private:
        QString m_filePath;
        DataReader *m_classHeaderReader;
        Dictionary m_headerDictionary;
        DataReader::DataBlockList m_classData;

        QByteArray m_header;
        QByteArray m_footer;
    };

}

#endif // CLASSSAVECARDLISTMODEL_H
