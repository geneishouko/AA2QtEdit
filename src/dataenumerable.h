#ifndef DATAENUMERABLE_H
#define DATAENUMERABLE_H

#include "datatype.h"

#include <QMap>

namespace ClassEdit {

    class DataEnumerable
    {
    public:
        DataEnumerable(const DataType &type);
        inline QString name(qint32 value) const {
            return m_strings[value];
        }
        DataType type() const;
        void registerStringValue(qint32 value, const QString &string);

    private:
        DataType m_type;
        QMap<qint32, QString> m_strings;
    };

}

#endif // DATAENUMERABLE_H
