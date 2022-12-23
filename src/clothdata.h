#ifndef CLOTHDATA_H
#define CLOTHDATA_H

#include "cardfile.h"
#include "dictionary.h"

#include <QObject>

namespace ClassEdit {

    class ClothData : public QObject
    {
    public:
        ClothData(QObject *parent = nullptr);

        static ClothData *fromCardData(const QString &type, CardFile *card);
        static ClothData *fromClothFile(const QString &file);
        static ClothData *fromByteArray(const QByteArray &byteArray);
        static ClothData *fromDevice(QIODevice *device);

        QVariant getValue(const QString &key) const;
        QVariantMap getValues(const QString &key) const;
        void setValue(const QString &key, const QVariant &value);
        QByteArray toClothFile() const;
    private:
        DataReader *m_dataReader;
        Dictionary m_dictionary;
    };

}

#endif // CLOTHDATA_H
