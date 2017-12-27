#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QHash>
#include <QObject>
#include <QVariant>

namespace ClassEdit {

    typedef QHash<QString,int> KeyIndex;

    class Dictionary : public QList<QVariant>
    {
    public:
        Dictionary();
        virtual ~Dictionary() = default;

        Dictionary filterByPrefix(const QString &prefix) const;
        QString keyAt(int index) const;
        void insert(const QString &key, const QVariant &value);
        void set(int index, const QVariant &value);
        QVariant value(const QString &key) const;

        KeyIndex m_keyMap;
    };

}

#endif // DICTIONARY_H
