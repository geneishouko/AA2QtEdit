#ifndef DATAREADER_H
#define DATAREADER_H

#include "datablock.h"
#include "dataenumerable.h"
#include "datastruct.h"

#include <QIODevice>
#include <QXmlStreamReader>
#include <QMap>
#include <QHash>
#include <QVariant>

namespace ClassEdit {

    class DataReader
    {
        friend class CardFile;
        friend class ClothData;
        friend class ClassSaveCardListModel;

        typedef QHash<QString, DataEnumerable*> DataEnumerableMap;
        typedef QHash<QString, DataStruct*> DataStructMap;
        typedef QHash<QString, DataBlock*> DataBlockMap;
        typedef QList<DataBlock*> DataBlockList;

    public:
        DataReader(QIODevice *xmlDefinition);
        static DataReader *getDataReader(const QString& name);
        DataEnumerable *getDataEnumerable(const QString& name) const;
        QVariant read(QIODevice *data, DataType type, int dataSize = 0) const;
        QVariant read(QIODevice *data, DataBlock *db) const;
        inline QVariant read(QIODevice *data, const QString &key) const {
                DataBlock *db = m_dataBlockMap[key];
                return read(data, db);
        }
        void write(QIODevice *data, const QString &key, const QVariant &value) const;
        void write(QIODevice *data, DataType type, const QVariant &value, int fieldSize) const;
        DataBlockList finalizeDataBlocks(QIODevice *data);
        void finalizeChildrenDataBlocks(QIODevice *data, DataBlock *db);
        static QByteArray decodeString(const QByteArray &data);

    private:
        bool loadDefinitions(QIODevice *xmlDefinition);
        void parseDefinitions(QXmlStreamReader &xml);
        void parseEnumerables(QXmlStreamReader &xml);
        void parseStructs(QXmlStreamReader &xml);
        void parseDatablocks(QXmlStreamReader &xml);

        DataStruct *getStruct(const QString &key) const;
        DataEnumerable *registerEnumerable(const QString &key, DataType type);
        DataStruct *registerStruct(const QString &key);

        DataEnumerableMap m_enumerables;
        DataStructMap m_structs;
        DataBlockMap m_dataBlockMap;
        DataBlockList m_dataBlocks;

        static QHash<QString, DataReader*> s_readers;
    };

}

#endif // DATAREADER_H
