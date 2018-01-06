#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "datablock.h"

#include <QHash>
#include <QObject>
#include <QSet>
#include <QVariant>

namespace ClassEdit {

    class DataBlock;
    class DataReader;

    class Dictionary : public QObject, public QList<QVariant>
    {
        Q_OBJECT
    public:

        enum ModelRoles {
            DataTypeRole = 0x0100,
            MetaKeyRole = 0x0101,
        };

        typedef QHash<QString,int> KeyIndex;
        typedef QSet<int> IndexSet;

        Dictionary(QObject *parent = nullptr);
        virtual ~Dictionary();

        void buildDisplayKeyList();

        inline const QList<DataBlock*> &dataBlocks() const {
            return m_dataBlockList;
        }

        const DataReader *dataReader();

        inline DataType dataType(int index) const {
            return m_dataBlockList.at(index)->type();
        }

        void deleteDataBlocks();

        inline IndexSet::Iterator dirtyIndexBegin() {
            return m_dirtyValues.begin();
        }

        IndexSet::Iterator dirtyIndexEnd() {
            return m_dirtyValues.end();
        }

        inline QStringList &displayKeyList() {
            return m_displayKeyList;
        }

        QString enumerable(int index);

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

        inline QString metaKey(int index) {
            return m_dataBlockList.at(index)->metaKey();
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

        inline void setDataReader(const DataReader *reader) {
            m_dataReader = reader;
        }

        inline void setDeleteDataBlocksOnDestruction(bool destroy) {
            m_deleteDataBlocks = destroy;
        }

        inline void setParentDictionary(Dictionary *parent) {
            setParent(parent);
            connect(this, &Dictionary::changed, parent, &Dictionary::childChanged);
        }

        QString typeName(int index) const;
        QVariant value(const QString &key) const;

    public slots:
        void childChanged();

    signals:
        void changed(int index);

    private:
        bool m_deleteDataBlocks;
        KeyIndex m_keyMap;
        QList<DataBlock*> m_dataBlockList;
        const DataReader *m_dataReader;
        IndexSet m_dirtyValues;
        QStringList m_displayKeyList;
    };

}

#endif // DICTIONARY_H
