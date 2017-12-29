#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "datatype.h"

#include <QXmlStreamAttributes>

namespace ClassEdit {

    class DataBlock
    {
        friend class DataReader;

    public:
        DataBlock();
        DataBlock(int currentOffset, QXmlStreamAttributes attr);
        DataBlock(const DataBlock &copy) = default;

        void setAttributes(int currentOffset, QXmlStreamAttributes attributes);
        bool isValid() const;

        inline QVector<DataBlock> &children() {
            return m_children;
        }

        QString key() const;
        int offset() const;
        int nextOffset() const;
        int dataSize() const;
        DataType type() const;
        QString typeName() const;
        QString metaKey() const;

    private:
        bool m_isValid;

        QString m_key;
        DataType m_type;
        int m_address;
        QString m_displayName;

        int m_minValue;
        int m_maxValue;
        int m_warnValue;

        int m_copyAddress;
        int m_dataSize;
        bool m_readOnly;

        DataType m_arrayInnerType;
        DataType m_arrayHeaderType;
        int m_arraySize;

        QString m_metaKey;

        QVector<DataBlock> m_children;
    };

}

#endif // DATABLOCK_H
