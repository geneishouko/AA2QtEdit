#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include "datablock.h"

#include <QVector>

namespace ClassEdit {

    class DataStruct
    {
    public:
        DataStruct();
        QVector<DataBlock> blocks() const;
        void registerDataBlock(const QXmlStreamAttributes &attributes);

    private:
        QVector<DataBlock> m_blocks;
    };

}

#endif // DATASTRUCT_H
