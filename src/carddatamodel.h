#ifndef CARDDATAMODEL_H
#define CARDDATAMODEL_H

#include "cardfile.h"

#include <QAbstractItemModel>
#include <QPersistentModelIndex>
#include <QHash>
#include <QVector>

namespace ClassEdit {
    class CardDataModelIndex;
    class CardDataModel : public QAbstractItemModel
    {
    public:
        CardDataModel(Dictionary *dictionary);

        int columnCount(const QModelIndex &parent) const;
        QVariant data(const QModelIndex &index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        Dictionary *getDictionaryForIndex(const QModelIndex &index) const;
        inline QVariant getEntryForIndex(const QModelIndex &index) const {
            return getDictionaryForIndex(index)->at(index.row());
        }
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    public slots:
        void updateRow(int row);

    private:
        Dictionary *m_dictionary;
    };

}

#endif // CARDDATAMODEL_H
