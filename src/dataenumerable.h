#ifndef DATAENUMERABLE_H
#define DATAENUMERABLE_H

#include "datatype.h"

#include <QMap>

namespace ClassEdit {

    class DataEnumerable
    {
    public:
        DataEnumerable(const DataType &type);
        int count() const {
            return m_strings.size();
        }
        inline QString name(qint32 value) const {
            auto it = m_strings.find(value);
            return it != m_strings.end() ? it.value() : QString::number(value, 10);
        }
        void registerStringValue(qint32 value, const QString &string);
        DataType type() const;
        qint32 value(int index) const {
            return m_strings.keys().at(index);
        }
        const QMap<qint32, QString> &valueMap() const {
            return m_strings;
        }

    private:
        DataType m_type;
        QMap<qint32, QString> m_strings;
    };

}

#endif // DATAENUMERABLE_H
