#ifndef CLOTHDATA_H
#define CLOTHDATA_H

#include "cardfile.h"

#include <QObject>

namespace ClassEdit {

    class ClothData : public QObject
    {
    public:
        ClothData(QObject *parent = nullptr);

        static ClothData *fromCardData(const QString &type, CardFile *card);
        static ClothData *fromClothFile(const QString &file);

        QVariant getValue(const QString &key) const;
        QHash<QString, QVariant> getValues(const QString &key) const;
        void setValue(const QString &key, const QVariant &value);
        QByteArray toClothFile() const;
    private:
        DataReader *m_dataReader;
        Dictionary *m_dictionary;
    };

}

#endif // CLOTHDATA_H
