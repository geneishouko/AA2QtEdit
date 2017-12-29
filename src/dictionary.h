#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "datablock.h"

#include <QHash>
#include <QObject>
#include <QSet>
#include <QVariant>

namespace ClassEdit {

    class DataBlock;

    class Dictionary : public QObject, public QList<QVariant>
    {
        Q_OBJECT
    public:
        typedef QHash<QString,int> KeyIndex;
        typedef QSet<int> IndexSet;

        Dictionary(QObject *parent = nullptr);
        virtual ~Dictionary() = default;

        void buildDisplayKeyList();

        inline DataType dataType(int index) const {
            return m_dataBlockList.at(index)->type();
        }

        inline IndexSet::Iterator dirtyIndexBegin() {
            return m_dirtyValues.begin();
        }

        IndexSet::Iterator dirtyIndexEnd() {
            return m_dirtyValues.end();
        }

        inline QStringList displayKeyList() const {
            return m_displayKeyList;
        }

        QVariantMap filterByPrefix(const QString &prefix) const;

        inline bool hasDirtyValues() const {
            return !m_dirtyValues.isEmpty();
        }

        inline QString keyAt(int index) const {
            return m_keyMap.key(index);
        }

        inline void insert(const QString &key, const QVariant &value) {
            m_keyMap.insertMulti(key, count());
            append(value);
        }

        inline int offset(int index) const {
            return m_dataBlockList.at(index)->offset();
        }

        inline void resetDirtyValues() {
            m_dirtyValues.clear();
        }

        void set(int index, const QVariant value);
        void set(const QString &key, QVariant value);
        void set(const QVariantMap &values);
        void setDataBlockList(QList<DataBlock*> blockList);
        QString typeName(int index) const;
        QVariant value(const QString &key) const;

    signals:
        void changed();

    private:
        KeyIndex m_keyMap;
        QList<DataBlock*> m_dataBlockList;
        IndexSet m_dirtyValues;
        QStringList m_displayKeyList;
    };

}

#endif // DICTIONARY_H
