#ifndef CARDDATAMODEL_H
#define CARDDATAMODEL_H

#include "cardfile.h"

#include <QAbstractListModel>

namespace ClassEdit {

    class CardDataModel : public QAbstractListModel
    {
    public:
        CardDataModel(CardFile* cardFile);

        int columnCount(const QModelIndex &parent) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent) const;
        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    private:
        CardFile* m_cardFile;
        static QVector<QString> s_keys;
    };

}

#endif // CARDDATAMODEL_H
